#ifndef CHIP_8_MEMORY
#define CHIP_8_MEMORY

#include "constants.hpp"

#include <array>
#include <cstdint>
#include <span>

namespace chip8
{

class Memory
{
  public:
    Memory();

    void load(std::span<uint8_t> values);

    [[nodiscard]] const std::array<uint8_t, memory::size>& get_data()
        const noexcept;
    [[nodiscard]] std::array<uint8_t, memory::size>& get_data() noexcept;

  private:
    std::array<uint8_t, memory::size> data_{};
};

inline const std::array<uint8_t, memory::size>& Memory::get_data()
    const noexcept
{
    return data_;
}

inline std::array<uint8_t, memory::size>& Memory::get_data() noexcept
{
    return data_;
}

}; // namespace chip8

#endif // CHIP_8_MEMORY