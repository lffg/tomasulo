#pragma once

#include <cstdint>
#include <ostream>
#include <string_view>
#include <vector>

namespace inst
{
    enum class op_class_t : uint8_t
    {
        additive,
        multiplicative,
        mem
    };

    std::ostream &operator<<(std::ostream &os, const op_class_t &op_class);

    enum class op_t : uint8_t
    {
        add,
        sub,
        mul,
        div,
        load,
        store
    };

    std::ostream &operator<<(std::ostream &os, const op_t &op);
    op_class_t class_for_op(op_t op);
    int latency_for_op(op_t);

    typedef struct inst_t
    {
        op_t op;
        uint8_t r1;
        uint8_t r2; /* may be offset */
        uint8_t r3; /* may be offset base */
    } inst_t;

    std::ostream &operator<<(std::ostream &os, const inst_t &inst);
    inst_t parse(std::string_view src, uint8_t reg_count);

    class prog_t
    {
    public:
        std::vector<inst_t> inst_list;

        prog_t() : inst_list{} {}
        void add(inst_t inst);
        inst_t &at(std::size_t i);
    };
}
