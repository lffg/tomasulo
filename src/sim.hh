#pragma once

#include <memory>

#include "reg.hh"
#include "station.hh"

namespace sim
{
    class ctx
    {
    public:
        std::shared_ptr<inst::prog_t> prog;
        std::size_t current_inst_num;
        reg_file_t regs;
        station_bag_t stations;
        int cycle;

        ctx(std::shared_ptr<inst::prog_t> prog, reg_file_t &&regs, station_bag_t &&stations)
            : prog(prog),
              current_inst_num(0),
              regs(std::move(regs)),
              stations(std::move(stations)),
              cycle(0) {}

        inst::inst_t &current_inst();
        void bump_inst();
    };
}
