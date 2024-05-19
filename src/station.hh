#pragma once

#include <cstdint>
#include <unordered_map>
#include <optional>
#include <vector>

#include "inst.hh"

class station_t
{
public:
    // The station ID.
    int id;
    // The station op class.
    inst::op_class_t op_class;
    // The operation being performed.
    inst::op_t op;
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
                  op_class{inst::op_class_t::additive}, op{inst::op_t::add},
                  qj{0}, qk{0}, vj{0}, vk{0}, a{0},
                  busy{false} {}
};

class station_bag_t
{
private:
    std::vector<station_t> stations;

public:
    station_bag_t() : stations{/* first is sentinel */ station_t{}} {}

    void add_station(inst::op_class_t op_class);

    // Returns a free reservation station for the given operator class, or NULL
    // if there isn't one.
    station_t *get_free(inst::op_class_t op_class);
};
