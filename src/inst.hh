#pragma once

#include <cstdint>
#include <ostream>
#include <string_view>

namespace inst
{
    typedef enum op_t
    {
        op_add,
        op_sub,
        op_mul,
        op_div,
        op_load,
        op_store,
    } __attribute__((__packed__)) op_t;

    typedef struct inst_t
    {
        op_t op;
        uint8_t r1;
        uint8_t r2; /* may be offset */
        uint8_t r3; /* may be offset base */
    } inst_t;

    inst_t parse(std::string_view src);
    std::ostream &operator<<(std::ostream &os, const inst_t &inst);
}
