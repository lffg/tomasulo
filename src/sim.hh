#pragma once

#include "reg.hh"
#include "station.hh"

namespace sim
{
    class ctx
    {
    public:
        reg_file_t regs;
        station_bag_t stations;
        int cycle;

        ctx(reg_file_t &&regs, station_bag_t &&stations)
            : regs(std::move(regs)),
              stations(std::move(stations)),
              cycle(1) {}
    };
}
