#include "sim.hh"
#include "inst.hh"

namespace sim
{
    inst::inst_t &ctx::current_inst()
    {
        return prog->at(current_inst_num);
    }

    void ctx::bump_inst()
    {
        current_inst_num++;
    }
}
