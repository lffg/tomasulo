#include "inst.hh"
#include "station.hh"

station_op_tracker_t::station_op_tracker_t(inst::op_t op)
{
    this->op = op;
    rem = inst::latency_for_op(op);
}

void station_bag_t::add_station(inst::op_class_t op_class)
{
    int id = static_cast<int>(stations.size());
    station_t s{};
    s.op_class = op_class;
    s.id = id;
    stations.push_back(s);
}

// Returns a free reservation station for the given operator class, or NULL
// if there isn't one.
station_t *station_bag_t::get_free(inst::op_class_t op_class)
{
    for (auto it = stations.begin() + 1; it != stations.end(); it++)
    {
        station_t *s = &*it;
        if (s->op_class != op_class || s->busy)
            continue;
        return s;
    }
    return nullptr;
}
