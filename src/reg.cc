#include "reg.hh"

bool reg_t::has_value()
{
    return qi != 0;
}

reg_file_t::reg_file_t(uint8_t amount)
{
    this->amount = amount;
    regs = std::vector<reg_t>{static_cast<std::size_t>(amount), reg_t{}};
}
