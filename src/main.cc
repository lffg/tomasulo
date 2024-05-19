#include <iostream>
#include <fstream>
#include <string>

#include "inst.hh"
#include "station.hh"

int main()
{
    std::ifstream f("prog/war-waw.txt");
    if (!f.is_open())
        throw std::runtime_error{"file does not exist"};

    inst::prog_t prog{};
    std::string line{};
    while (std::getline(f, line))
    {
        prog.add(inst::parse(line));
    }

    station_bag_t station_bag{};
    station_bag.add_station(inst::op_class_t::additive);
    station_bag.add_station(inst::op_class_t::additive);
    station_bag.add_station(inst::op_class_t::additive);
    station_bag.add_station(inst::op_class_t::additive);
    station_bag.add_station(inst::op_class_t::multiplicative);
    station_bag.add_station(inst::op_class_t::multiplicative);
    station_bag.add_station(inst::op_class_t::mem);
    station_bag.add_station(inst::op_class_t::mem);

    std::cout << "Hello, world!\n";

    // inst::inst_t i = inst::parse("sw r255,0(r66)");
    // std::cout << "[" << i << "]\n";

    return 0;
}
