#pragma once

#include <cstdint>
#include <unordered_map>
#include <optional>
#include <vector>

#include "inst.hh"

// An operation tracker.
class station_op_tracker_t
{
public:
    inst::op_t op;
    // How many cycles remain to finish execution.
    int rem;

    station_op_tracker_t(inst::op_t op);
};

class station_t
{
public:
    // The station ID.
    uint8_t id;
    // The station op class.
    inst::op_class_t op_class;
    // An operation tracker (used for the simulation).
    // In a normal operation, a simple operation discriminant would suffice.
    station_op_tracker_t tracker;
    // The reservation station that will produce the relevant source operand.
    // If zero, corresponding value is in `vj` or `vk`.
    uint8_t qj, qk;
    // The value fo the source operands (if any).
    uint32_t vj, vk;
    // Memory address information for a load or a store.
    uint8_t a;
    // Whether the station is currently in use.
    bool busy;

    station_t() : id{0},
                  op_class{inst::op_class_t::additive},
                  tracker{station_op_tracker_t{inst::op_t::add}},
                  qj{0}, qk{0}, vj{0}, vk{0}, a{0},
                  busy{false} {}

    void reset()
    {
        *this = station_t{};
    }

    void write_j_value(uint32_t value);
    void write_k_value(uint32_t value);
};

class station_bag_t
{
private:
    std::vector<station_t> stations;

public:
    station_bag_t() : stations{/* first is sentinel */ station_t{}} {}

    void add_station(inst::op_class_t op_class);

    auto begin() { return stations.begin() + 1; }
    auto end() { return stations.end(); }

    station_t &at(uint8_t i);

    // Returns a free reservation station for the given operator class, or NULL
    // if there isn't one.
    station_t *get_free(inst::op_class_t op_class);

    void show(std::ostream &os);
};
