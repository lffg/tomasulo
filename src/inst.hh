#pragma once

#include <cstdint>
#include <ostream>
#include <string_view>

namespace inst
{
    enum class op_class_t : uint8_t
    {
        additive,
        multiplicative,
        mem
    };

    enum class op_t : uint8_t
    {
        add,
        sub,
        mul,
        div,
        load,
        store
    };

    typedef struct inst_t
    {
        op_t op;
        uint8_t r1;
        uint8_t r2; /* may be offset */
        uint8_t r3; /* may be offset base */
    } inst_t;

    op_class_t class_for_op(op_t op);
    int latency_for_op(op_t);
    inst_t parse(std::string_view src);
    std::ostream &operator<<(std::ostream &os, const inst_t &inst);
}
