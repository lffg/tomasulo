#pragma once

#include <vector>
#include <cstdint>
#include <ostream>

class reg_t
{
public:
    uint32_t value;
    // Reservation station whose result should be stored in this register.
    // Zero if `value` is already in this register.
    uint8_t qi;

    reg_t() : value{1}, qi{0} {}
    bool has_value();
    bool waiting_on_station();
    // Writes a value and marks qi as 0.
    void write_value(uint32_t value);
    uint8_t station_id();
};

class reg_file_t
{
public:
    uint8_t amount;
    std::vector<reg_t> regs;

    reg_file_t(uint8_t amount);
    reg_t &at(uint8_t i);
    void show(std::ostream &os);

    auto begin() { return regs.begin(); }
    auto end() { return regs.end(); }
};
