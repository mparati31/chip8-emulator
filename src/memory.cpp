#include "memory.hpp"

#include "constants.hpp"

#include <algorithm>
#include <cstdint>
#include <span>

namespace chip8
{

Memory::Memory()
{
    std::ranges::copy(font::built_in, data_.begin() + memory::font_address);
}

void Memory::load(std::span<uint8_t> values)
{
    std::ranges::copy(values, data_.begin() + memory::free_address);
}

} // namespace chip8
