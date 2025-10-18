#include "chip8.hpp"

#include "constants.hpp"
#include "cpu.hpp"
#include "display.hpp"
#include "io_manager.hpp"
#include "memory.hpp"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <expected>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace chip8
{

Chip8::Chip8(std::shared_ptr<IOManager> io, uint16_t rate)
    : io_{std::move(io)},
      mem_{std::make_shared<Memory>()},
      display_{std::make_shared<Display>(io_)},
      cpu_{std::make_shared<Cpu>(io_, mem_, display_)},
      cpu_rate_{rate}
{
}

Chip8::Chip8(Chip8 const&) noexcept = default;

Chip8::Chip8(Chip8&&) noexcept = default;

Chip8::~Chip8() = default;

Chip8& Chip8::operator=(Chip8 const&) noexcept = default;

Chip8& Chip8::operator=(Chip8&&) noexcept = default;

std::expected<void, LoadRomError> Chip8::load_rom(std::string const& path)
{
    assert(!running_);

    if (std::ifstream file(path, std::ios::binary | std::ios::ate);
        file.is_open())
    {
        auto size = file.tellg();
        if (size == 0)
        {
            return std::unexpected(LoadRomError::ROM_EMPTY);
        }
        if (size > memory::rom_max_size)
        {
            return std::unexpected(LoadRomError::ROM_TOO_BIG);
        }

        std::vector<uint8_t> buffer(size);
        file.seekg(0, std::ios::beg);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        auto* char_buffer = reinterpret_cast<char*>(buffer.data());
        file.read(char_buffer, size);

        mem_->load(buffer);

        rom_loaded_ = true;
    }
    else
    {
        return std::unexpected(LoadRomError::FILE_NOT_FOUND);
    }

    return {};
}

void Chip8::start()
{
    assert(rom_loaded_);
    assert(!running_);

    io_->start();

    running_ = true;
    run_main_loop();
}

void Chip8::run_main_loop()
{
    using namespace std::chrono_literals;

    using clock    = std::chrono::high_resolution_clock;
    using duration = std::chrono::duration<double>;

    assert(running_);

    const duration cpu_frame_duration(1.0 / cpu_rate_);
    auto cpu_accumulator    = duration::zero();
    auto timers_accumulator = duration::zero();
    auto prev_time          = clock::now();

    while (running_)
    {
        const bool quit = !io_->update();
        if (quit)
        {
            io_->stop();
            running_ = false;
            continue;
        }

        auto current_time = clock::now();
        const auto dt     = current_time - prev_time;

        cpu_accumulator += dt;
        timers_accumulator += dt;

        if (timers_accumulator >= timer::frame_duration)
        {
            cpu_->update_timers();
            timers_accumulator -= timer::frame_duration;
        }

        if (cpu_accumulator >= cpu_frame_duration)
        {
            cpu_->tick();
            display_->print();
            cpu_accumulator -= cpu_frame_duration;
        }

        prev_time = current_time;

        std::this_thread::sleep_for(loop::duration);
    }
}

} // namespace chip8
