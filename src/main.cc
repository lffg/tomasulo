#include <iostream>
#include <fstream>
#include <string>

#include "inst.hh"
#include "station.hh"
#include "reg.hh"

int main()
{
    std::ifstream f("prog/war-waw.txt");
    if (!f.is_open())
        throw std::runtime_error{"file does not exist"};

    uint8_t reg_count = 32;

    inst::prog_t prog{};
    std::string line{};
    while (std::getline(f, line))
    {
        prog.add(inst::parse(line, reg_count));
    }

    reg_file_t reg_file{reg_count};

    station_bag_t station_bag{};
    station_bag.add_station(inst::op_class_t::additive);
    station_bag.add_station(inst::op_class_t::additive);
    station_bag.add_station(inst::op_class_t::additive);
    station_bag.add_station(inst::op_class_t::additive);
    station_bag.add_station(inst::op_class_t::multiplicative);
    station_bag.add_station(inst::op_class_t::multiplicative);
    station_bag.add_station(inst::op_class_t::mem);
    station_bag.add_station(inst::op_class_t::mem);

    station_bag.show(std::cout);

    std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";

    station_t *s = station_bag.get_free(inst::op_class_t::multiplicative);
    if (s == nullptr)
    {
        std::cout << "<!!!@@@!!!> NOT found\n";
    }

    s->fill_station(inst::op_t::div);

    station_bag.show(std::cout);

    return 0;
}
