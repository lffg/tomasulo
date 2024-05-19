#include <vector>
#include <cstdint>

class reg_t
{
public:
    uint32_t value;
    // Reservation station whose result should be stored in this register.
    // Zero if `value` is already in this register.
    int qi;

    reg_t() : value{0}, qi{0} {}
    bool has_value();
};

class reg_file_t
{
public:
    uint8_t amount;
    std::vector<reg_t> regs;

    reg_file_t(uint8_t amount);
};
