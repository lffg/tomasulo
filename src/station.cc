#include "inst.hh"
#include "station.hh"

station_op_tracker_t::station_op_tracker_t(inst::op_t o)
{
    op = o;
    rem = inst::latency_for_op(op);
}

void station_t::write_j_value(uint32_t value)
{
    vj = value;
    qj = 0;
}
void station_t::write_k_value(uint32_t value)
{
    vk = value;
    qk = 0;
}

void station_bag_t::add_station(inst::op_class_t op_class)
{
    int id = static_cast<int>(stations.size());
    station_t s{};
    s.op_class = op_class;
    s.id = id;
    stations.push_back(s);
}

station_t &station_bag_t::at(uint8_t i)
{
    return stations.at(static_cast<std::size_t>(i));
}

// Returns a free reservation station for the given operator class, or NULL
// if there isn't one.
station_t *station_bag_t::get_free(inst::op_class_t op_class)
{
    for (auto it = begin(); it != end(); it++)
    {
        station_t *s = &(*it);
        if (s->op_class != op_class || s->busy)
            continue;
        return s;
    }
    return nullptr;
}

void station_bag_t::show(std::ostream &os)
{
    os << "| Kind\t | Id\t | Busy\t | Op\t | Qj\t | Vj\t | Qk\t | Vk\t | A\t |";
    os << "\n";
    for (auto it = begin(); it != end(); it++)
    {
        station_t &s = *it;
        os << "| " << s.op_class << "\t ";             // kind
        os << "| " << static_cast<int>(s.id) << "\t "; // id
        os << "| " << s.busy << "\t ";                 // busy
        if (s.busy)
        {
            os << "| " << s.tracker.op << "\t ";           // op
            os << "| " << static_cast<int>(s.qj) << "\t "; // qj
            os << "| ";                                    // start vj
            if (s.qj == 0)
                os << static_cast<int>(s.vj); // actual vj
            else
                os << "n/a";
            os << "\t ";
            os << "| " << static_cast<int>(s.qk) << "\t "; // qk
            os << "| ";                                    // start vk
            if (s.qk == 0)
                os << static_cast<int>(s.vk); // actual vk
            else
                os << "n/a";
            os << "\t ";
            os << "| " << static_cast<int>(s.a) << "\t "; // a
        }
        else
        {
            for (int i = 0; i < 6; i++)
            {
                os << "|\t ";
            }
        }
        os << "|\n";
    }
}
