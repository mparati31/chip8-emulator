#ifndef CHIP_8_UTILITY
#define CHIP_8_UTILITY

#include <array>
#include <cstdint>
#include <string>
#include <variant>

namespace chip8::utility
{

template <typename T, std::size_t Height, std::size_t Width>
using matrix = std::array<std::array<T, Width>, Height>;

struct Color
{
    uint8_t r{};
    uint8_t g{};
    uint8_t b{};
    uint8_t a{};
};

static constexpr Color white{.r = 255, .g = 255, .b = 255, .a = 255};
static constexpr Color black{.r = 0, .g = 0, .b = 0, .a = 255};

uint8_t random_byte();

namespace argparse
{

struct Options
{
    std::string rom;
    uint16_t rate{};
};

struct EmptyOptions
{
};
static inline constexpr EmptyOptions empty_options;

enum class ParseError : uint8_t
{
    MissingRom,
    ParseError
};

using ParseResult = std::variant<Options, EmptyOptions, ParseError>;

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
ParseResult parse(int argc, char* argv[]);

} // namespace argparse
} // namespace chip8::utility

#endif // CHIP_8_UTILITY