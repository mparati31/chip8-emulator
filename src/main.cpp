#include "chip8.hpp"
#include "sdl2manager.hpp"
#include "utility.hpp"

#include <cstdlib>
#include <expected>
#include <iostream>
#include <memory>
#include <optional>
#include <print>
#include <variant>

namespace
{

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace chip8::utility;

bool handle_load_rom_result(std::expected<void, chip8::LoadRomError> res)
{
    if (res.has_value())
    {
        std::println("ROM loaded successfully");
        return true;
    }

    switch (res.error())
    {
        using enum chip8::LoadRomError;
    case FILE_NOT_FOUND:
        std::println(std::cerr, "Error: ROM not found");
        break;
    case ROM_EMPTY:
        std::println(std::cerr, "Error: ROM is empty");
        break;
    case ROM_TOO_BIG:
        std::println(std::cerr, "Error: ROM exceeds maximum size");
        break;
    }
    return false;
}

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
std::optional<argparse::Options> parse_args(int argc, char* argv[], int& ret)
{
    // NOLINTNEXTLINE(google-build-using-namespace)
    using namespace argparse;

    std::variant parse_res = parse(argc, argv);
    auto* opts             = std::get_if<Options>(&parse_res);
    if (opts)
    {
        return std::move(*opts);
    }

    if (std::holds_alternative<ParseError>(parse_res))
    {
        ret = EXIT_FAILURE;
    }
    return std::nullopt;
}

int run_emulator(const chip8::utility::argparse::Options& opts)
{
    auto io = std::make_shared<chip8::Sdl2Manager>();
    chip8::Chip8 emulator(io, opts.rate);

    if (auto load_res = emulator.load_rom(opts.rom);
        !handle_load_rom_result(load_res))
    {
        return EXIT_FAILURE;
    }

    emulator.start();

    return EXIT_SUCCESS;
}

} // namespace

int main(int argc, char* argv[])
{
    auto ret = EXIT_SUCCESS;

    if (const auto parse_res = parse_args(argc, argv, ret))
    {
        // NOLINTNEXTLINE(hicpp-signed-bitwise)
        ret |= run_emulator(*parse_res);
    }

    return ret;
}
