#include <iostream> // TODO REMOVE THIS
#include <cassert>

#include "sim.hh"
#include "inst.hh"

void issue(sim::ctx &ctx);
void execute(sim::ctx &ctx);
void write(sim::ctx &ctx);

namespace sim
{
    bool ctx::has_next_inst()
    {
        return pc < prog->inst_list.size();
    }

    // Callers are encouraged to call `has_next_inst` before.
    inst::inst_t &ctx::next_inst()
    {
        return prog->at(pc);
    }

    void ctx::bump_inst()
    {
        pc++;
    }

    void simulate_iteration(ctx &ctx)
    {
        ctx.cycle += 1;

        issue(ctx);
        execute(ctx);
        write(ctx);
    }
}

void fill_non_mem_station(sim::ctx &ctx, inst::inst_t &inst, station_t *r)
{
    // if the register is waiting on a station result, mark this reservation
    // station as waiting for *that* reservation station
    reg_t &op1_reg = ctx.regs.at(inst.r2);
    if (op1_reg.waiting_on_station() /* i.e., [Qi != 0] */)
        r->qj = op1_reg.station_id();
    else
    {
        r->vj = op1_reg.value;
        r->qj = 0;
    }

    // same as above, but for the other operand
    reg_t &op2_reg = ctx.regs.at(inst.r3);
    if (op2_reg.waiting_on_station())
        r->qk = op2_reg.station_id();
    else
    {
        r->vk = op2_reg.value;
        r->qk = 0;
    }

    // mark the register record for this instruction's *destination* register
    // with the id of the reservation station that is being used to track this
    // instruction's execution
    ctx.regs.at(inst.r1).qi = r->id;
}

void fill_mem_station(sim::ctx &ctx, inst::inst_t &inst, station_t *r)
{
    // e.g. lw r1,r2(r3)
    // e.g. sw r1,r2(r3)
    //
    // in the canonical description (see Patterson's CAaQA book),
    // [r1 is referred to as rt], [r3 as rs], and [r2 as imm].

    // both load and store instructions need to read from the `r3` (base addr)
    // to compute the effective address. same remarks as above applies.
    reg_t &op_reg = ctx.regs.at(inst.r3);
    if (op_reg.waiting_on_station())
        r->qj = op_reg.station_id();
    else
    {
        r->vj = op_reg.value;
        r->qj = 0;
    }

    // the offset is immediately available
    r->a = inst.r2;

    if (inst.op == inst::op_t::load)
    // in a load, r1 is WRITTEN to
    {
        // mark the register record for this instruction's *destination*
        // register (which will be loaded from memory) with the id of the
        // reservation station that is being used to track this load's execution
        ctx.regs.at(inst.r1).qi = r->id;
    }
    else // is store
    // in a store, r1 is READ from
    {
        reg_t &src_op_reg = ctx.regs.at(inst.r1);
        if (src_op_reg.waiting_on_station())
            r->qk = src_op_reg.station_id();
        else
        {
            r->vk = src_op_reg.value;
            r->qk = 0;
        }
    }
}

void issue(sim::ctx &ctx)
{
    if (!ctx.has_next_inst())
        return;

    inst::inst_t &inst = ctx.next_inst();

    // try to get the reservation station, `r`, for the given instruction
    // operation class
    inst::op_class_t op_class = inst::class_for_op(inst.op);
    station_t *r = ctx.stations.get_free(op_class);
    // if there isn't an available reservation station for the instruction's
    // operation class, we halt the issue of new instructions to avoid a
    // structural hazard
    if (r == nullptr)
        return;

    // bump the pc after a successful issue-start
    ctx.bump_inst();

    r->busy = true;
    r->tracker.op = inst.op;
    r->tracker.rem = inst::latency_for_op(inst.op);
    if (op_class == inst::op_class_t::mem)
        fill_mem_station(ctx, inst, r);
    else
        fill_non_mem_station(ctx, inst, r);
}

void execute_non_mem_inst(station_t &r)
{
    // in order to *start* executing, both operands must be valid.
    // if that's not the case, bail out.
    if (!(r.qj == 0 && r.qk == 0))
        return;

    r.tracker.rem -= 1; // emulate latency
}

void execute_mem_inst(station_t &r)
{
    if (!(r.qj == 0))
        return;

    // since we only have one memory unit, we can emulate the concept of a
    // load-store queue, so we preserve the correct program ordering behavior
    if (r.tracker.op == inst::op_t::store)
    { // apparently, we don't need to wait for `r1` (the value to be stored)
      // due to the existence of such a load-store queue.
    }

    // execute load/store step 1
    r.a = r.vj + r.a;
    // (some latency here...)

    // execute load/store step 2 after step 1 (effective addr calculation) gets
    // finished
    // (read from Mem[r.a]...)

    r.tracker.rem -= 1; // emulate latency
}

void execute(sim::ctx &ctx)
{
    // now we're on a given clock cycle number.
    //
    // the hardware is implemented such that all stations *can* make progress
    // in parallel. we simulate this behavior on this software by iterating over
    // all *busy* reservation stations and checking to see if any can make
    // progress.
    for (station_t &r : ctx.stations)
    {
        if (!r.busy)
            continue;

        if (r.op_class == inst::op_class_t::mem)
            execute_mem_inst(r);
        else
            execute_non_mem_inst(r);
    }
}

uint32_t get_computed_val(station_t &r)
{
    assert(r.qj == 0);
    assert(r.qk == 0);
    switch (r.tracker.op)
    {
    case inst::op_t::add:
        return r.vj + r.vk;
    case inst::op_t::sub:
        return r.vj - r.vk;
    case inst::op_t::mul:
        return r.vj * r.vk;
    case inst::op_t::div:
        if (r.vk == 0)
            return UINT32_MAX;
        else
            return r.vj / r.vk;
    case inst::op_t::load:
        return r.a; // return the effective address
    case inst::op_t::store:
        return 0; // no-op
    }
}

void write(sim::ctx &ctx)
{
    // same remarks as above.
    // iterate through all busy stations that *also* have exhausted the
    // remaining tracker steps (latency simulator).
    for (station_t &r : ctx.stations)
    {
        if (!r.busy || 0 < r.tracker.rem)
            continue;

        // the side-effect of a store isn't implemented on this simulator (since
        // we don't have the concept of memory).
        // as no values be propagated (bus on the real impl.), we early return.
        if (r.tracker.op == inst::op_t::store)
        {
            r.reset();
            return;
        }

        // we do the actual computation here (on the software) since we don't
        // have a bus to actually propagate the just-computed values. that's
        // not a problem, though, as we're only getting this result *after* the
        // execution phase is completed (and the simulated latency has passed).
        uint32_t new_val = get_computed_val(r);

        // propagate the computed value for all interested registers and
        // reservation stations
        for (reg_t &reg : ctx.regs)
            if (!reg.has_value() && reg.qi == r.id)
            {
                std::cout << "for r.id=" << r.id << ", and reg.qi=" << reg.qi << ".\n";
                reg.write_value(new_val);
            }
        for (station_t &s : ctx.stations)
        {
            if (s.qj == r.id)
                s.write_j_value(new_val);
            if (s.qk == r.id)
                s.write_k_value(new_val);
        }

        // now, this station doesn't have any more use. mark it as free
        r.reset();
    }
}
