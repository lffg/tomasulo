#include <iostream>
#include <fstream>
#include <string>

#include "inst.hh"

int main()
{
    std::cout << "Hello, world!\n";

    std::ifstream f("prog/war-waw.txt");
    if (!f.is_open())
        throw std::runtime_error{"file does not exist"};

    std::string line{};
    while (std::getline(f, line))
    {
        if (line == "")
            continue;
        std::cout << "got line [" << line << "]";
        inst::inst_t i = inst::parse(line);
        std::cout << " -> parsed as [" << i << "]\n";
    }

    inst::inst_t i = inst::parse("sw r255,0(r66)");
    std::cout << "[" << i << "]\n";

    return 0;
}
