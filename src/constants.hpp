#ifndef CHIP_8_CONSTANTS
#define CHIP_8_CONSTANTS

#include <array>
#include <chrono>
#include <cstdint>

namespace chip8
{

namespace loop
{

using namespace std::chrono_literals;

constexpr auto duration = 100us;

} // namespace loop

namespace timer
{

constexpr std::size_t fps     = 60;
constexpr auto frame_duration = std::chrono::duration<double>(1.f / fps);

} // namespace timer

namespace cpu
{

constexpr uint8_t n_registers = 16;
constexpr uint8_t stack_size  = 16;

} // namespace cpu

namespace memory
{

constexpr uint8_t byte = 8;

constexpr uint16_t size         = 4096;
constexpr uint16_t font_address = 0x050;
constexpr uint16_t free_address = 0x200;

constexpr uint8_t instruction_size = 2;

constexpr uint16_t rom_max_size = size - free_address;

} // namespace memory

namespace display
{

constexpr uint8_t height_size = 32;
constexpr uint8_t width_size  = 64;

constexpr uint8_t pixel_scale = 15;

} // namespace display

namespace input
{

constexpr uint8_t n_keys = 16;

} // namespace input

namespace font
{

constexpr uint8_t n_letters   = 16;
constexpr uint8_t letter_size = 5;

// NOLINTNEXTLINE(bugprone-implicit-widening-of-multiplication-result)
constexpr std::array<uint8_t, n_letters * letter_size> built_in{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

} // namespace font

namespace sprite
{

constexpr uint8_t width = 8;

} // namespace sprite

namespace mask
{

constexpr uint16_t less_significant_bit  = 0x01u;
constexpr uint16_t most_significant_bit  = 0x80u;
constexpr uint16_t most_significant_byte = 0xff00u;

constexpr uint16_t type = 0xf000;
constexpr uint16_t x    = 0x0f00;
constexpr uint16_t y    = 0x00f0;
constexpr uint16_t n    = 0x000f;
constexpr uint16_t nn   = y | n;
constexpr uint16_t nnn  = x | nn;

constexpr uint16_t type_8 = 0xf00f;
constexpr uint16_t type_e = 0xf0ff;
constexpr uint16_t type_f = 0xf0ff;

} // namespace mask

namespace offset
{

constexpr uint8_t type = 12;
constexpr uint8_t x    = 8;
constexpr uint8_t y    = 4;

}; // namespace offset

namespace instruction
{

constexpr uint16_t empty = 0x0000;

constexpr uint16_t type_0 = 0x0000;
constexpr uint16_t type_8 = 0x8000;
constexpr uint16_t type_e = 0xe000;
constexpr uint16_t type_f = 0xf000;

// NOLINTBEGIN(readability-identifier-naming)
constexpr uint16_t _0nnn = 0x0000;
constexpr uint16_t _00e0 = 0x00e0;
constexpr uint16_t _00ee = 0x00ee;
constexpr uint16_t _1nnn = 0x1000;
constexpr uint16_t _2nnn = 0x2000;
constexpr uint16_t _3xkk = 0x3000;
constexpr uint16_t _4xkk = 0x4000;
constexpr uint16_t _5xy0 = 0x5000;
constexpr uint16_t _6xkk = 0x6000;
constexpr uint16_t _7xkk = 0x7000;
constexpr uint16_t _8xy0 = 0x8000;
constexpr uint16_t _8xy1 = 0x8001;
constexpr uint16_t _8xy2 = 0x8002;
constexpr uint16_t _8xy3 = 0x8003;
constexpr uint16_t _8xy4 = 0x8004;
constexpr uint16_t _8xy5 = 0x8005;
constexpr uint16_t _8xy6 = 0x8006;
constexpr uint16_t _8xy7 = 0x8007;
constexpr uint16_t _8xye = 0x800e;
constexpr uint16_t _9xy0 = 0x9000;
constexpr uint16_t _annn = 0xa000;
constexpr uint16_t _bnnn = 0xb000;
constexpr uint16_t _cxkk = 0xc000;
constexpr uint16_t _dxyn = 0xd000;
constexpr uint16_t _ex9e = 0xe09e;
constexpr uint16_t _exa1 = 0xe0a1;
constexpr uint16_t _fx07 = 0xf007;
constexpr uint16_t _fx0a = 0xf00a;
constexpr uint16_t _fx15 = 0xf015;
constexpr uint16_t _fx18 = 0xf018;
constexpr uint16_t _fx1e = 0xf01e;
constexpr uint16_t _fx29 = 0xf029;
constexpr uint16_t _fx33 = 0xf033;
constexpr uint16_t _fx55 = 0xf055;
constexpr uint16_t _fx65 = 0xf065;
// NOLINTEND(readability-identifier-naming)

} // namespace instruction

} // namespace chip8

#endif // CHIP_8_CONSTANTS