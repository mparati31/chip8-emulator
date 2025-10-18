#include "sdl2manager.hpp"
#include "SDL_keycode.h"
#include "constants.hpp"

#include <SDL_events.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <print>
#include <vector>

namespace
{

// Defined as a global variable and not as a class member because it must be
// accessible from inside the callback in the setup_beep function, i.e. a
// c-style function pointer and therefore the lambda used cannot capture values.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
SDL_AudioDeviceID g_audio_device;

} // namespace

namespace chip8
{

Sdl2Manager::Sdl2Manager() noexcept = default;

Sdl2Manager::~Sdl2Manager()
{
    if (running_)
    {
        stop();
    }
}

bool Sdl2Manager::start()
{
    running_ = init() && create_window() && create_renderer();
    setup_beep();
    return running_;
}

bool Sdl2Manager::update()
{
    assert(running_);

    SDL_Event event;
    // NOLINTNEXTLINE(readability-implicit-bool-conversion)
    while (SDL_PollEvent(&event))
    {
        const auto key = event.key.keysym.sym;

        if (event.type == SDL_QUIT ||
            event.type == SDL_KEYDOWN && key == SDLK_ESCAPE)
        {
            return false;
        }

        if (event.type == SDL_KEYDOWN)
        {
            pressed_keys_.insert(key);
        }
        else if (event.type == SDL_KEYUP)
        {
            pressed_keys_.erase(key);
        }
    }

    return true;
}

void Sdl2Manager::stop() noexcept
{
    if (!running_)
    {
        return;
    }

    if (renderer_)
    {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_)
    {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    if (g_audio_device != 0)
    {
        SDL_CloseAudioDevice(g_audio_device);
        g_audio_device = 0;
    }
    SDL_Quit();
}

void Sdl2Manager::fetch_keys(std::array<bool, chip8::input::n_keys>& out_keys,
                             bool additive)
{
    if (!additive)
    {
        std::ranges::fill(out_keys, false);
    }

    for (const auto key : pressed_keys_)
    {
        switch (key)
        {
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,
        //             readability-magic-numbers)
        // clang-format off
            case '1': out_keys[0x1] = true; break;
            case '2': out_keys[0x2] = true; break;
            case '3': out_keys[0x3] = true; break;
            case '4': out_keys[0xc] = true; break;
            case 'q': out_keys[0x4] = true; break;
            case 'w': out_keys[0x5] = true; break;
            case 'e': out_keys[0x6] = true; break;
            case 'r': out_keys[0xd] = true; break;
            case 'a': out_keys[0x7] = true; break;
            case 's': out_keys[0x8] = true; break;
            case 'd': out_keys[0x9] = true; break;
            case 'f': out_keys[0xe] = true; break;
            case 'z': out_keys[0xa] = true; break;
            case 'x': out_keys[0x0] = true; break;
            case 'c': out_keys[0xb] = true; break;
            case 'v': out_keys[0xf] = true; break;
        // clang-format on
        // NOLINTEND
        default:
        }
    }
}

void Sdl2Manager::render(
    utility::matrix<bool, display::height_size, display::width_size> const&
        pixels) noexcept
{
    assert(running_);

    SDL_SetRenderDrawColor(renderer_, config_.background.r,
                           config_.background.g, config_.background.b,
                           config_.background.a);
    SDL_RenderClear(renderer_);
    SDL_SetRenderDrawColor(renderer_, config_.foreground.r,
                           config_.foreground.g, config_.foreground.b,
                           config_.foreground.a);

    for (int y = 0; y < display::height_size; ++y)
    {
        for (int x = 0; x < display::width_size; ++x)
        {
            if (pixels[y][x])
            {
                SDL_Rect pixel_rect;
                pixel_rect.x = x * display::pixel_scale;
                pixel_rect.y = y * display::pixel_scale;
                pixel_rect.w = display::pixel_scale;
                pixel_rect.h = display::pixel_scale;

                SDL_RenderFillRect(renderer_, &pixel_rect);
            }
        }
    }

    SDL_RenderPresent(renderer_);
}

// NOLINTNEXTLINE(readability-make-member-function-const)
void Sdl2Manager::play_beep() noexcept
{
    assert(running_);
    assert(g_audio_device != 0);

    SDL_PauseAudioDevice(g_audio_device, 0);
}

bool Sdl2Manager::init() noexcept
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        std::print(std::cerr, "SDL_Init failed: {}", SDL_GetError());
        stop();
        return false;
    }
    return true;
}

bool Sdl2Manager::create_window() noexcept
{
    constexpr int window_width  = display::width_size * display::pixel_scale;
    constexpr int window_height = display::height_size * display::pixel_scale;

    // NOLINTBEGIN(hicpp-signed-bitwise)
    window_ = SDL_CreateWindow(PROGRAM_NAME " " PROGRAM_VERSION,
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               window_width, window_height, SDL_WINDOW_SHOWN);
    // NOLINTEND(hicpp-signed-bitwise)
    if (!window_)
    {
        std::print(std::cerr, "SDL_CreateWindow failed: {}", SDL_GetError());
        stop();
        return false;
    }
    return true;
}

bool Sdl2Manager::create_renderer() noexcept
{
    assert(window_);

    renderer_ = SDL_CreateRenderer(
        window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_)
    {
        std::print(std::cerr, "SDL_CreateRenderer failed: {}", SDL_GetError());
        stop();
        return false;
    }
    return true;
}

// NOLINTNEXTLINE(readability-make-member-function-const)
void Sdl2Manager::setup_beep() noexcept
{
    assert(running_);

    constexpr int sample_rate = 44100;
    constexpr int samples     = 2048;
    constexpr float freq      = 440.0f;
    constexpr float volume    = 0.2f;
    constexpr float two_pi    = 6.28318531f;
    SDL_AudioSpec want        = {};
    want.freq                 = sample_rate;
    want.format               = AUDIO_F32SYS;
    want.channels             = 1;
    want.samples              = samples;
    want.callback             = [](void*, Uint8* stream, int len) {
        static float phase = 0.0f;
        size_t n_samples   = static_cast<size_t>(len) / sizeof(float);
        std::vector<float> buffer(n_samples);
        for (size_t i = 0; i < n_samples; ++i)
        {
            buffer[i] = volume * sinf(phase);
            phase += two_pi * freq / sample_rate;
            if (phase > two_pi)
            {
                phase -= two_pi;
            }
        }
        memcpy(stream, buffer.data(), n_samples * sizeof(float));

        SDL_PauseAudioDevice(g_audio_device, 1);
    };
    want.userdata = nullptr;

    // Setup the audio device and immediately pause them in order to avoid to
    // reproduce the sound now.
    g_audio_device = SDL_OpenAudioDevice(nullptr, 0, &want, nullptr, 0);
    SDL_PauseAudioDevice(g_audio_device, 1);
}

} // namespace chip8
