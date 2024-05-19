#include <cctype>
#include <charconv>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "inst.hh"

// syntax: <ins>
//
// <ins>            ::= <op_name> " " <reg> "," <regs_or_offset>
// <reg>            ::= "r" [0-9]+
// <regs_or_offset> ::= <regs> | <offset>
// <regs>           ::= <reg> "," <reg>
// <offset>         ::= [0-9]+ "(" <reg> ")"
// <op_name>        ::= "add" | "sub" | "mul" | "div" | "sw" | "lw"

inst::inst_t zeroed_inst()
{
    inst::inst_t i;
    i.op = inst::op_t::add;
    i.r1 = 0;
    i.r2 = 0;
    i.r3 = 0;
    return i;
}

class instruction_parser
{
private:
    std::string_view src;
    size_t i;
    uint8_t reg_count;

    inst::op_t parse_op()
    {
        size_t len = 0;
        while (isalpha(src[i + len]))
            len++;
        auto sub = src.substr(i, len);
        i += len;

        if (sub == "add")
            return inst::op_t::add;
        if (sub == "sub")
            return inst::op_t::sub;
        if (sub == "mul")
            return inst::op_t::mul;
        if (sub == "div")
            return inst::op_t::div;
        if (sub == "lw")
            return inst::op_t::load;
        if (sub == "sw")
            return inst::op_t::store;
        throw std::runtime_error{"unexpected instruction name"};
    }

    uint8_t parse_uint8()
    {
        size_t len = 0;
        while (isdigit(src[i + len]))
            len++;
        auto sub = src.substr(i, len);
        i += len;

        uint8_t n = 0;
        auto [ptr, ec] = std::from_chars(sub.data(), sub.data() + sub.size(), n);

        if (ec == std::errc::result_out_of_range)
            throw std::runtime_error{"invalid number: out of range"};
        else if (ec != std::errc())
            throw std::runtime_error{"invalid number"};
        return n;
    }

    uint8_t parse_reg()
    {
        if (src[i++] != 'r')
            throw std::runtime_error{"expected `r` for register"};
        uint8_t reg_no = parse_uint8();
        if (reg_count <= reg_no)
            throw std::runtime_error("tried to use invalid register (too big)");
        return reg_no;
    }

    std::pair<uint8_t, uint8_t> parse_offset()
    {
        uint8_t a = parse_uint8();
        if (src[i++] != '(')
            throw std::runtime_error{"expected `(`"};
        uint8_t b = parse_reg();
        if (src[i++] != ')')
            throw std::runtime_error{"expected `)`"};
        return std::pair{a, b};
    }

public:
    instruction_parser(std::string_view src, uint8_t reg_count) : src(src), i(0), reg_count(reg_count) {}

    inst::inst_t parse()
    {
        inst::inst_t inst = zeroed_inst();
        inst.op = parse_op();

        if (src[i++] != ' ')
            throw std::runtime_error{"expected space after instruction name"};

        inst.r1 = parse_reg();

        if (src[i++] != ',')
            throw std::runtime_error{"expected comma (1)"};

        if (src[i] == 'r')
        {
            inst.r2 = parse_reg();
            if (src[i++] != ',')
                throw std::runtime_error{"expected comma (2)"};
            inst.r3 = parse_reg();
        }
        else
        {
            auto [r2, r3] = parse_offset();
            inst.r2 = r2;
            inst.r3 = r3;
        }

        if (i != src.size())
            throw std::runtime_error{"excepted eof"};

        return inst;
    }
};

namespace inst
{
    op_class_t class_for_op(op_t op)
    {
        switch (op)
        {
        case op_t::add:
        case op_t::sub:
            return op_class_t::additive;
        case op_t::mul:
        case op_t::div:
            return op_class_t::multiplicative;
        case op_t::load:
        case op_t::store:
            return op_class_t::mem;
        }
    }

    int latency_for_op(op_t op)
    {
        switch (op)
        {
        case op_t::add:
        case op_t::sub:
            return 2;
        case op_t::mul:
            return 8;
        case op_t::div:
            return 12;
        case op_t::load:
        case op_t::store:
            return 3;
        }
    }

    inst_t parse(std::string_view src, uint8_t reg_count)
    {
        instruction_parser p{src, reg_count};
        return p.parse();
    }

    std::ostream &operator<<(std::ostream &os, const inst_t &inst)
    {
        // Yeah, I know this isn't right since it doesn't follow the actual
        // instruction encoding, but it works for the sake of this project,
        // as we're assuming that `lw` and `sw` instructions always use the
        // immediate-as-offset form.
        switch (inst.op)
        {
        case op_t::add:
            os << "add ";
            break;
        case op_t::sub:
            os << "sub ";
            break;
        case op_t::mul:
            os << "mul ";
            break;
        case op_t::div:
            os << "div ";
            break;
        case op_t::load:
            os << "lw ";
            break;
        case op_t::store:
            os << "sw ";
            break;
        }
        switch (inst.op)
        {
        case op_t::add:
        case op_t::sub:
        case op_t::mul:
        case op_t::div:
            os << "r" << static_cast<int>(inst.r1) << "," << "r" << static_cast<int>(inst.r2) << "," << "r" << static_cast<int>(inst.r3);
            break;
        case op_t::load:
        case op_t::store:
            os << "r" << static_cast<int>(inst.r1) << "," << static_cast<int>(inst.r2) << "(" << "r" << static_cast<int>(inst.r3) << ")";
            break;
        }
        return os;
    }

    void prog_t::add(inst_t i)
    {
        prog.push_back(i);
    }
}
