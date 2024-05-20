#include "reg.hh"

bool reg_t::has_value()
{
    return qi == 0;
}

bool reg_t::waiting_on_station()
{
    return !has_value();
}

void reg_t::write_value(uint32_t value)
{
    this->value = value;
    qi = 0;
}

uint8_t reg_t::station_id()
{
    if (has_value())
        throw std::logic_error{"called station_id but has value"};
    return qi;
}

reg_file_t::reg_file_t(uint8_t amount)
{
    this->amount = amount;
    regs = std::vector<reg_t>{static_cast<std::size_t>(amount), reg_t{}};
}

reg_t &reg_file_t::at(uint8_t i)
{
    return regs.at(static_cast<std::size_t>(i));
}

void reg_file_t::show(std::ostream &os)
{
    os << "|";
    for (std::size_t i = 0; i < regs.size(); i++)
    {
        os << " reg" << i << "\t|";
    }
    os << "\n|";
    for (std::size_t i = 0; i < regs.size(); i++)
    {
        reg_t &reg = regs.at(i);
        os << " ";
        if (reg.has_value())
        {
            os << reg.value;
        }
        else
        {
            os << "->" << static_cast<int>(reg.station_id());
        }
        os << "\t|";
    }
    os << "\n";
}
