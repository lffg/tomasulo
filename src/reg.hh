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

    reg_t() : value{0}, qi{0} {}
    bool has_value();
    uint8_t station_id();
};

class reg_file_t
{
public:
    uint8_t amount;
    std::vector<reg_t> regs;

    reg_file_t(uint8_t amount);
    void show(std::ostream &os);
};
