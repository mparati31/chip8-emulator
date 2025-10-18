#include "display.hpp"

#include "constants.hpp"
#include "io_manager.hpp"
#include "utility.hpp"

#include <algorithm>
#include <cstdint>

namespace chip8
{

Display::Display(std::shared_ptr<IOManager> io) : io_{std::move(io)}
{
}

void Display::clear() noexcept
{
    for (auto& row : display_)
    {
        std::ranges::fill(row, false);
    }
}

bool Display::draw(uint8_t coord_x, uint8_t coord_y,
                   std::span<const uint8_t> sprite) noexcept
{
    bool is_any_pixel_turned_off = false;
    int n_sprite_rows            = 0;
    for (const auto sprite_row : sprite)
    {
        for (uint8_t i = 0; i < sprite::width; ++i)
        {
            const uint8_t y = coord_y + n_sprite_rows;
            const uint8_t x = coord_x + i;

            if (display::height_size <= y)
            {
                return is_any_pixel_turned_off;
            }
            if (display::width_size <= x)
            {
                continue;
            }

            auto& pixel           = display_[y][x];
            const auto prev_pixel = pixel;
            // NOLINTNEXTLINE(hicpp-signed-bitwise)
            pixel ^= (sprite_row >> (sprite::width - i - 1)) & 1;
            is_any_pixel_turned_off |= prev_pixel && !pixel;
        }

        ++n_sprite_rows;
    }
    updated_ = true;

    return is_any_pixel_turned_off;
}

void Display::print()
{
    if (!updated_)
    {
        return;
    }

    io_->render(display_);

    updated_ = false;
}

} // namespace chip8
