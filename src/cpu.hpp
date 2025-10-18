#ifndef CHIP_8_CPU
#define CHIP_8_CPU

#include "constants.hpp"

#include <array>
#include <cstdint>
#include <memory>

namespace chip8
{

class IOManager;
class Memory;
class Display;

class Cpu
{
  public:
    Cpu(std::shared_ptr<IOManager> io, std::shared_ptr<Memory> mem,
        std::shared_ptr<Display> display);

    void tick();

    void update_timers();

  private:
    void log_opcode_error() const noexcept;

    void fetch() noexcept;
    void execute() noexcept;

    void exec_sys_addr() noexcept;
    void exec_cls() noexcept;
    void exec_ret();
    void exec_jp_nnn() noexcept;
    void exec_call();
    void exec_se_vx_nn() noexcept;
    void exec_sne_vx_nn() noexcept;
    void exec_se_vx_vy() noexcept;
    void exec_ld_vx_nn() noexcept;
    void exec_add_vx_nn() noexcept;
    void exec_ld_vx_vy() noexcept;
    void exec_or_vx_vy() noexcept;
    void exec_and_vx_vy() noexcept;
    void exec_xor_vx_vy() noexcept;
    void exec_add_vx_vy() noexcept;
    void exec_sub_vx_vy() noexcept;
    void exec_shr_vx_vy() noexcept;
    void exec_subn_vx_vy() noexcept;
    void exec_shl_vx_vy() noexcept;
    void exec_sne_vx_vy() noexcept;
    void exec_ld_i_nnn() noexcept;
    void exec_jp_v0_nnn() noexcept;
    void exec_rnd_vx_nn() noexcept;
    void exec_drw_vx_vy_n() noexcept;
    void exec_skp_vx() noexcept;
    void exec_nskp_vx() noexcept;
    void exec_ld_vx_dt() noexcept;
    void exec_ld_vx_k();
    void exec_ld_dt_vx() noexcept;
    void exec_ld_st_vx() noexcept;
    void exec_add_i_vx() noexcept;
    void exec_ld_f_vx() noexcept;
    void exec_ld_b_vx();
    void exec_ld_i_vx();
    void exec_ld_vx_i();

    [[nodiscard]] uint8_t get_n() const noexcept;
    [[nodiscard]] uint8_t get_nn() const noexcept;
    [[nodiscard]] uint16_t get_nnn() const noexcept;

    [[nodiscard]] uint8_t get_x() const noexcept;
    [[nodiscard]] uint8_t get_y() const noexcept;

    [[nodiscard]] uint8_t get_vx() const noexcept;
    [[nodiscard]] uint8_t& get_vx() noexcept;
    [[nodiscard]] uint8_t& get_vy() noexcept;

    [[nodiscard]] uint8_t& get_v0() noexcept;
    [[nodiscard]] uint8_t& get_vf() noexcept;

    [[nodiscard]] bool is_key_vx_pressed() const noexcept;

    std::shared_ptr<IOManager> io_;

    std::shared_ptr<Memory> mem_;
    std::shared_ptr<Display> display_;

    std::array<uint8_t, cpu::n_registers> registers_{};
    uint16_t index_{};
    uint16_t pc_{memory::free_address};
    std::array<uint16_t, cpu::stack_size> stack_{};
    int8_t stack_ptr_{-1};

    uint8_t delay_timer_{};
    uint8_t sound_timer_{};

    std::array<bool, input::n_keys> keys_{};

    uint16_t opcode_{};
};

inline uint8_t Cpu::get_n() const noexcept
{
    return opcode_ & mask::n;
}

inline uint8_t Cpu::get_nn() const noexcept
{
    return opcode_ & mask::nn;
}

inline uint16_t Cpu::get_nnn() const noexcept
{
    return opcode_ & mask::nnn;
}

inline uint8_t Cpu::get_x() const noexcept
{
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    return (opcode_ & mask::x) >> offset::x;
}

inline uint8_t Cpu::get_y() const noexcept
{
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    return (opcode_ & mask::y) >> offset::y;
}

inline uint8_t Cpu::get_vx() const noexcept
{
    const uint8_t x = get_x();
    return registers_[x];
}

inline uint8_t& Cpu::get_vx() noexcept
{
    const uint8_t x = get_x();
    return registers_[x];
}

inline uint8_t& Cpu::get_vy() noexcept
{
    const uint8_t y = get_y();
    return registers_[y];
}

inline uint8_t& Cpu::get_v0() noexcept
{
    return registers_[0x0];
}

inline uint8_t& Cpu::get_vf() noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    return registers_[0xf];
}

inline bool Cpu::is_key_vx_pressed() const noexcept
{
    const auto vx = get_vx();
    return keys_[vx];
}

} // namespace chip8

#endif // CHIP_8_CPU