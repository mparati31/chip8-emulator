#ifndef CHIP_8_SDL_2_MANAGER
#define CHIP_8_SDL_2_MANAGER

#include "constants.hpp"
#include "io_manager.hpp"
#include "utility.hpp"

#include <SDL2/SDL.h>
#include <unordered_set>

namespace chip8
{

struct Sdl2ManagerConfig
{
    utility::Color background = utility::black;
    utility::Color foreground = utility::white;
};

class Sdl2Manager : public IOManager
{
  public:
    using Config = Sdl2ManagerConfig;

    Sdl2Manager() noexcept;
    Sdl2Manager(const Sdl2Manager&) = delete;
    Sdl2Manager(Sdl2Manager&&)      = delete;

    ~Sdl2Manager() override;

    Sdl2Manager& operator=(const Sdl2Manager&) = delete;
    Sdl2Manager& operator=(Sdl2Manager&&)      = delete;

    [[nodiscard]] Config get_config() const noexcept;
    [[nodiscard]] Config get_config() noexcept;

    [[nodiscard]] bool is_running() const noexcept override;

    bool start() override;
    bool update() override;
    void stop() noexcept override;

    void fetch_keys(std::array<bool, chip8::input::n_keys>& out_keys,
                    bool additive) override;

    void render(
        utility::matrix<bool, display::height_size, display::width_size> const&
            pixels) noexcept override;

    void play_beep() noexcept override;

  private:
    bool init() noexcept;
    bool create_window() noexcept;
    bool create_renderer() noexcept;
    void setup_beep() noexcept;

    SDL_Window* window_{nullptr};
    SDL_Renderer* renderer_{nullptr};
    std::unordered_set<SDL_Keycode> pressed_keys_;

    bool running_{false};

    Config config_{};
};

inline Sdl2Manager::Config Sdl2Manager::get_config() const noexcept
{
    return config_;
};

inline Sdl2Manager::Config Sdl2Manager::get_config() noexcept
{
    return config_;
}

[[nodiscard]] inline bool Sdl2Manager::is_running() const noexcept
{
    return running_;
}

} // namespace chip8

#endif // CHIP_8_SDL_2_MANAGER
