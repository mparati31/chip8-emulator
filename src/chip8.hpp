#ifndef CHIP_8
#define CHIP_8

#include <expected>
#include <memory>

namespace chip8
{

class Cpu;
class Memory;
class Display;
class IOManager;

enum class LoadRomError : uint8_t
{
    FILE_NOT_FOUND,
    ROM_EMPTY,
    ROM_TOO_BIG
};

class Chip8
{
  public:
    Chip8(std::unique_ptr<IOManager> io, uint16_t rate);
    Chip8(Chip8 const&) = delete;
    Chip8(Chip8&&) noexcept;

    ~Chip8();

    Chip8& operator=(Chip8 const&) = delete;
    Chip8& operator=(Chip8&&) noexcept;

    std::expected<void, LoadRomError> load_rom(std::string const& path);

    void start();

  private:
    void run_main_loop();

    std::unique_ptr<IOManager> io_;

    std::unique_ptr<Memory> mem_;
    std::unique_ptr<Display> display_;
    std::unique_ptr<Cpu> cpu_;

    uint16_t cpu_rate_;

    bool rom_loaded_{false};
    bool running_{false};
};

} // namespace chip8

#endif // CHIP_8
