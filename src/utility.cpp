#include "utility.hpp"

#include <cstdint>
#include <cxxopts.hpp>
#include <fcntl.h>
#include <iostream>
#include <limits>
#include <print>
#include <random>
#include <string>
#include <termios.h>
#include <unistd.h>

namespace chip8::utility
{

uint8_t random_byte()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint8_t> dist(
        std::numeric_limits<uint8_t>::min(),
        std::numeric_limits<uint8_t>::max());
    auto number = dist(gen);
    return static_cast<uint8_t>(number);
}

namespace argparse
{

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
ParseResult parse(int argc, char* argv[])
{
    cxxopts::Options options(PROGRAM_NAME, PROGRAM_NAME " " PROGRAM_VERSION);

    constexpr std::string_view rom_opt     = "rom";
    constexpr std::string_view rate_opt    = "rate";
    constexpr std::string_view help_opt    = "help";
    constexpr std::string_view version_opt = "version";

    // NOLINTBEGIN(bugprone-suspicious-stringview-data-usage)

    // clang-format off
    options.add_options()
        (std::string("r,") + rate_opt.data(), "Instructions per second",
            cxxopts::value<uint16_t>()->default_value("500"))
        (std::string("f,") + rom_opt.data(), "Path to the ROM file to load",
            cxxopts::value<std::string>())
        (std::string("h,") + help_opt.data(), "Print help information")
        (std::string("v,") + version_opt.data(), "Print version information");
    // clang-format on

    options.parse_positional({rom_opt.data()});
    options.positional_help("<rom>");

    try
    {
        auto result = options.parse(argc, argv);

        if (result.contains(help_opt.data()))
        {
            std::print("{}", options.help());
            return empty_options;
        }

        if (result.contains(version_opt.data()))
        {
            std::println("{} {}", PROGRAM_NAME, PROGRAM_VERSION);
            return empty_options;
        }

        if (!result.contains(rom_opt.data()))
        {
            std::print(std::cerr,
                       "Error: ROM is required, use --help for more info\n");
            return ParseError::MissingRom;
        }

        return Options{.rom  = result[rom_opt.data()].as<std::string>(),
                       .rate = result[rate_opt.data()].as<uint16_t>()};

        // NOLINTEND(bugprone-suspicious-stringview-data-usage)
    }
    catch (const std::exception& e)
    {
        std::println(std::cerr, "Error parsing arguments: {}", e.what());
        return ParseError::ParseError;
    }
}

} // namespace argparse

} // namespace chip8::utility
