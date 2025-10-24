#ifndef CHIP_8_DISPLAY
#define CHIP_8_DISPLAY

#include "constants.hpp"
#include "utility.hpp"

#include <span>

namespace chip8
{

class IOManager;

class Display
{
  public:
    explicit Display(IOManager* io);

    void clear() noexcept;

    bool draw(uint8_t coord_x, uint8_t coord_y,
              std::span<const uint8_t> sprite) noexcept;

    void print();

  private:
    IOManager* io_;

    utility::matrix<bool, display::height_size, display::width_size> display_{};

    bool updated_{false};
};

} // namespace chip8

#endif // CHIP_8_DISPLAY
