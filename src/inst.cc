#include <cctype>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <charconv>
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
    i.op = inst::op_add;
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

    inst::op_t parse_op()
    {
        size_t len = 0;
        while (isalpha(src[i + len]))
            len++;
        auto sub = src.substr(i, len);
        i += len;

        if (sub == "add")
            return inst::op_add;
        if (sub == "sub")
            return inst::op_sub;
        if (sub == "mul")
            return inst::op_mul;
        if (sub == "div")
            return inst::op_div;
        if (sub == "lw")
            return inst::op_load;
        if (sub == "sw")
            return inst::op_store;
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
        return parse_uint8();
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
    instruction_parser(std::string_view src) : src(src), i(0) {}

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

    inst_t parse(std::string_view src)
    {
        instruction_parser p{src};
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
        case op_add:
            os << "add ";
            break;
        case op_sub:
            os << "sub ";
            break;
        case op_mul:
            os << "mul ";
            break;
        case op_div:
            os << "div ";
            break;
        case op_load:
            os << "lw ";
            break;
        case op_store:
            os << "sw ";
            break;
        }
        switch (inst.op)
        {
        case op_add:
        case op_sub:
        case op_mul:
        case op_div:
            os << "r" << static_cast<int>(inst.r1) << "," << "r" << static_cast<int>(inst.r2) << "," << "r" << static_cast<int>(inst.r3);
            break;
        case op_load:
        case op_store:
            os << "r" << static_cast<int>(inst.r1) << "," << static_cast<int>(inst.r2) << "(" << "r" << static_cast<int>(inst.r3) << ")";
            break;
        }
        return os;
    }
}