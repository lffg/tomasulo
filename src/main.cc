#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "inst.hh"
#include "station.hh"
#include "reg.hh"
#include "sim.hh"

const uint8_t REG_COUNT = 8;

void clear_stdout();
void print_divider();

sim::ctx create_simulator_ctx(std::string_view path)
{
    std::ifstream f{path.data()};
    if (!f.is_open())
        throw std::runtime_error{"file does not exist"};

    inst::prog_t prog{};
    std::string line{};
    while (std::getline(f, line))
    {
        if (line.empty() || line.at(0) == ';')
            continue;
        prog.add(inst::parse(line, REG_COUNT));
    }

    reg_file_t reg_file{REG_COUNT};

    station_bag_t station_bag{};
    station_bag.add_station(inst::op_class_t::additive);
    station_bag.add_station(inst::op_class_t::additive);
    station_bag.add_station(inst::op_class_t::additive);
    station_bag.add_station(inst::op_class_t::additive);
    station_bag.add_station(inst::op_class_t::multiplicative);
    station_bag.add_station(inst::op_class_t::multiplicative);
    station_bag.add_station(inst::op_class_t::mem);
    // notice that we only have one memory unit to emulate the concept of the
    // "load-store queue".

    sim::ctx ctx{std::make_shared<inst::prog_t>(prog), std::move(reg_file), std::move(station_bag)};
    return ctx;
}

void print_ctx(sim::ctx &ctx)
{
    std::cout << "Showing cycle number (" << ctx.cycle << ")\n";
    if (ctx.has_next_inst())
        std::cout << "Current instruction (#" << ctx.pc << ") is [" << ctx.next_inst() << "]\n";
    else
        std::cout << "Current instruction is [N/A]\n";

    print_divider();
    ctx.stations.show(std::cout);
    print_divider();
    ctx.regs.show(std::cout);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "error: must provide path to instructions file as argument.\n";
        return 1;
    }
    std::string_view instructions_file_path{argv[1]};

    int first = true;

    std::vector<sim::ctx> ctx_history{create_simulator_ctx(instructions_file_path)};
    std::size_t current_cycle = ctx_history.back().cycle;
outer:
    do
    {
        // We want to see warnings!
        if (!first)
            clear_stdout();
        first = false;

        if (current_cycle == ctx_history.size())
        {
            // copy last
            ctx_history.push_back(ctx_history.back());
            // run simulation for the newly-created context
            sim::simulate_iteration(ctx_history.back());
        }

        print_ctx(ctx_history.at(current_cycle));

        print_divider();
        std::cout << "[n]ext, [p]rev, [q]uit => ";
        while (true)
        {
            char answer = 'q';
            std::cin >> answer;
            switch (answer)
            {
            case 'n':
                current_cycle++;
                goto outer;
            case 'p':
                if (0 < current_cycle)
                    current_cycle--;
                goto outer;
            case 'q':
                goto outer_end;
            default:
                std::cout << "Again => ";
                continue;
            }
        }
    } while (0);
outer_end:

    std::cout << "bye!\n";

    return 0;
}

void clear_stdout()
{
#ifdef WINDOWS
    auto _ = system("cls");
#else
    std::cout << "\033c";
#endif
}

void print_divider()
{
    std::cout << "==================================================================================\n";
}
