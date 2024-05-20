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
        std::size_t pc;
        reg_file_t regs;
        station_bag_t stations;
        int cycle;

        ctx(std::shared_ptr<inst::prog_t> prog, reg_file_t &&regs, station_bag_t &&stations)
            : prog(prog),
              pc(0),
              regs(std::move(regs)),
              stations(std::move(stations)),
              cycle(0) {}

        bool has_next_inst();
        inst::inst_t &next_inst();
        void bump_inst();
    };

    void execute(ctx &ctx);
}
