#ifndef CHIP8_IO_MANAGER
#define CHIP8_IO_MANAGER

#include "constants.hpp"
#include "utility.hpp"

namespace chip8
{

class IOManager
{
  public:
    IOManager();
    IOManager(IOManager const&) = delete;
    IOManager(IOManager&&)      = delete;

    virtual ~IOManager();

    IOManager& operator=(IOManager const&) = delete;
    IOManager& operator=(IOManager&&)      = delete;

    [[nodiscard]] virtual bool is_running() const noexcept = 0;

    virtual void fetch_keys(std::array<bool, chip8::input::n_keys>& out_keys,
                            bool additive) = 0;

    virtual bool start()  = 0;
    virtual bool update() = 0;
    virtual void stop()   = 0;

    virtual void render(utility::matrix<bool, display::height_size,
                                        display::width_size> const& pixels) = 0;
    virtual void play_beep()                                                = 0;
};

} // namespace chip8

#endif // CHIP8_IO_MANAGER