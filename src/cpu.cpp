#include "cpu.hpp"

#include "constants.hpp"
#include "display.hpp"
#include "io_manager.hpp"
#include "memory.hpp"
#include "utility.hpp"

#include <cstdint>
#include <memory>
#include <print>
#include <ranges>
#include <span>
#include <utility>

namespace chip8
{

Cpu::Cpu(std::shared_ptr<IOManager> io, std::shared_ptr<Memory> mem,
         std::shared_ptr<Display> display)
    : io_{std::move(io)}, mem_{std::move(mem)}, display_{std::move(display)}
{
}

void Cpu::tick()
{
    io_->fetch_keys(keys_, false);
    fetch();
    execute();
}

void Cpu::update_timers()
{
    if (delay_timer_ > 0)
    {
        --delay_timer_;
    }

    if (sound_timer_ > 0)
    {
        io_->play_beep();
        --sound_timer_;
    }
}

void Cpu::log_opcode_error() const noexcept
{
    std::println(stderr, "Unknown or unsupported opcode: 0x{:04X}", opcode_);
}

void Cpu::fetch() noexcept
{
    const auto& mem = mem_->get_data();
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    opcode_ = (mem[pc_] << memory::byte) | mem[pc_ + 1];
    pc_ += memory::instruction_size;
}

void Cpu::execute() noexcept
{
    if (opcode_ == instruction::empty)
    {
        std::println("Empty instruction skipped");
        return;
    }

    switch (opcode_ & mask::type)
    {
    case instruction::type_0:
        switch (opcode_)
        {
        case instruction::_00e0:
            exec_cls();
            break;
        case instruction::_00ee:
            exec_ret();
            break;
        case instruction::_0nnn:
            exec_sys_addr();
            break;
        default:
            log_opcode_error();
            break;
        }
        break;
    case instruction::_1nnn:
        exec_jp_nnn();
        break;
    case instruction::_2nnn:
        exec_call();
        break;
    case instruction::_3xkk:
        exec_se_vx_nn();
        break;
    case instruction::_4xkk:
        exec_sne_vx_nn();
        break;
    case instruction::_5xy0:
        exec_se_vx_vy();
        break;
    case instruction::_6xkk:
        exec_ld_vx_nn();
        break;
    case instruction::_7xkk:
        exec_add_vx_nn();
        break;
    case instruction::type_8:
        switch (opcode_ & mask::type_8)
        {
        case instruction::_8xy0:
            exec_ld_vx_vy();
            break;
        case instruction::_8xy1:
            exec_or_vx_vy();
            break;
        case instruction::_8xy2:
            exec_and_vx_vy();
            break;
        case instruction::_8xy3:
            exec_xor_vx_vy();
            break;
        case instruction::_8xy4:
            exec_add_vx_vy();
            break;
        case instruction::_8xy5:
            exec_sub_vx_vy();
            break;
        case instruction::_8xy6:
            exec_shr_vx_vy();
            break;
        case instruction::_8xy7:
            exec_subn_vx_vy();
            break;
        case instruction::_8xye:
            exec_shl_vx_vy();
            break;
        default:
            log_opcode_error();
            break;
        }
        break;
    case instruction::_9xy0:
        exec_sne_vx_vy();
        break;
    case instruction::_annn:
        exec_ld_i_nnn();
        break;
    case instruction::_bnnn:
        exec_jp_v0_nnn();
        break;
    case instruction::_cxkk:
        exec_rnd_vx_nn();
        break;
    case instruction::_dxyn:
        exec_drw_vx_vy_n();
        break;
    case instruction::type_e:
        switch (opcode_ & mask::type_e)
        {
        case instruction::_ex9e:
            exec_skp_vx();
            break;
        case instruction::_exa1:
            exec_nskp_vx();
            break;
        default:
            log_opcode_error();
            break;
        }
        break;
    case instruction::type_f:
        switch (opcode_ & mask::type_f)
        {
        case instruction::_fx07:
            exec_ld_vx_dt();
            break;
        case instruction::_fx0a:
            exec_ld_vx_k();
            break;
        case instruction::_fx15:
            exec_ld_dt_vx();
            break;
        case instruction::_fx18:
            exec_ld_st_vx();
            break;
        case instruction::_fx1e:
            exec_add_i_vx();
            break;
        case instruction::_fx29:
            exec_ld_f_vx();
            break;
        case instruction::_fx33:
            exec_ld_b_vx();
            break;
        case instruction::_fx55:
            exec_ld_i_vx();
            break;
        case instruction::_fx65:
            exec_ld_vx_i();
            break;
        default:
            log_opcode_error();
            break;
        }
        break;
    default:
        break;
    }
}

void Cpu::exec_sys_addr() noexcept
{
    log_opcode_error();
}

void Cpu::exec_cls() noexcept
{
    display_->clear();
}

void Cpu::exec_ret()
{
    pc_ = stack_[stack_ptr_--];
}

void Cpu::exec_jp_nnn() noexcept
{
    pc_ = get_nnn();
}

void Cpu::exec_call()
{
    stack_[++stack_ptr_] = pc_;
    pc_                  = get_nnn();
}

void Cpu::exec_se_vx_nn() noexcept
{
    const auto vx = get_vx();
    const auto nn = get_nn();
    if (vx == nn)
    {
        pc_ += memory::instruction_size;
    }
}

void Cpu::exec_sne_vx_nn() noexcept
{
    const auto vx = get_vx();
    const auto nn = get_nn();
    if (vx != nn)
    {
        pc_ += memory::instruction_size;
    }
}

void Cpu::exec_se_vx_vy() noexcept
{
    const auto vx = get_vx();
    const auto vy = get_vy();
    if (vx == vy)
    {
        pc_ += memory::instruction_size;
    }
}

void Cpu::exec_ld_vx_nn() noexcept
{
    auto& vx = get_vx();
    vx       = get_nn();
}

void Cpu::exec_add_vx_nn() noexcept
{
    auto& vx = get_vx();
    vx += get_nn();
}

void Cpu::exec_ld_vx_vy() noexcept
{
    auto& vx = get_vx();
    vx       = get_vy();
}

void Cpu::exec_or_vx_vy() noexcept
{
    auto& vx = get_vx();
    vx |= get_vy();
}

void Cpu::exec_and_vx_vy() noexcept
{
    auto& vx = get_vx();
    vx &= get_vy();
}

void Cpu::exec_xor_vx_vy() noexcept
{
    auto& vx = get_vx();
    vx ^= get_vy();
}

void Cpu::exec_add_vx_vy() noexcept
{
    auto& vx      = get_vx();
    auto& vf      = get_vf();
    const auto vy = get_vy();

    const uint16_t sum  = vx + vy;
    vx                  = static_cast<uint8_t>(sum);
    const auto overflow = (sum & mask::most_significant_byte) > 0;
    vf                  = static_cast<uint8_t>(overflow);
}

void Cpu::exec_sub_vx_vy() noexcept
{
    auto& vx      = get_vx();
    auto& vf      = get_vf();
    const auto vy = get_vy();

    const uint8_t sub = vx - vy;
    const auto borrow = vx >= vy;
    vx                = sub;
    vf                = static_cast<uint16_t>(borrow);
}

void Cpu::exec_shr_vx_vy() noexcept
{
    auto& vx          = get_vx();
    auto& vf          = get_vf();
    const uint8_t lsb = vx & mask::less_significant_bit;
    vx >>= 1u;
    vf = lsb;
}

void Cpu::exec_subn_vx_vy() noexcept
{
    auto& vx      = get_vx();
    auto& vf      = get_vf();
    const auto vy = get_vy();

    const uint8_t sub = vy - vx;
    const auto borrow = vy >= vx;
    vx                = sub;
    vf                = static_cast<uint8_t>(borrow);
}

void Cpu::exec_shl_vx_vy() noexcept
{
    auto& vx = get_vx();
    auto& vf = get_vf();
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    const uint8_t msb = (vx & mask::most_significant_bit) >> 7;
    vx <<= 1u;
    vf = msb;
}

void Cpu::exec_sne_vx_vy() noexcept
{
    const auto vx = get_vx();
    const auto vy = get_vy();
    if (vx != vy)
    {
        pc_ += memory::instruction_size;
    }
}

void Cpu::exec_ld_i_nnn() noexcept
{
    index_ = get_nnn();
}

void Cpu::exec_jp_v0_nnn() noexcept
{
    const auto v0  = get_v0();
    const auto nnn = get_nnn();
    pc_            = v0 + nnn;
}

void Cpu::exec_rnd_vx_nn() noexcept
{
    auto& vx          = get_vx();
    const auto nn     = get_nn();
    const auto random = utility::random_byte();
    vx                = random & nn;
}

void Cpu::exec_drw_vx_vy_n() noexcept
{
    const auto vx     = get_vx();
    const auto vy     = get_vy();
    const auto n      = get_n();
    const auto sprite = std::span{mem_->get_data()}.subspan(index_, n);
    auto& vf          = get_vf();
    vf                = display_->draw(vx, vy, sprite) ? 1 : 0;
}

void Cpu::exec_skp_vx() noexcept
{
    if (is_key_vx_pressed())
    {
        pc_ += memory::instruction_size;
    }
}

void Cpu::exec_nskp_vx() noexcept
{
    if (!is_key_vx_pressed())
    {
        pc_ += memory::instruction_size;
    }
}

void Cpu::exec_ld_vx_dt() noexcept
{
    auto& vx = get_vx();
    vx       = delay_timer_;
}

void Cpu::exec_ld_vx_k()
{
    auto* it = std::ranges::find_if(keys_, std::identity{});
    if (it == keys_.end())
    {
        pc_ -= memory::instruction_size;
    }
    else
    {
        auto& vx = get_vx();
        vx       = std::distance(keys_.begin(), it);
    }
}

void Cpu::exec_ld_dt_vx() noexcept
{
    delay_timer_ = get_vx();
}

void Cpu::exec_ld_st_vx() noexcept
{
    sound_timer_ = get_vx();
}

void Cpu::exec_add_i_vx() noexcept
{
    index_ += get_vx();
}

void Cpu::exec_ld_f_vx() noexcept
{

    const auto vx = get_vx();
    index_        = memory::font_address + vx * font::letter_size;
}

void Cpu::exec_ld_b_vx()
{
    const auto vx = get_vx();
    auto& mem     = mem_->get_data();
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    mem[index_]     = (vx / 100) % 10;
    mem[index_ + 1] = (vx / 10) % 10;
    mem[index_ + 2] = vx % 10;
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
}

void Cpu::exec_ld_i_vx()
{
    const auto x         = get_x();
    const auto registers = registers_ | std::ranges::views::take(x + 1);
    auto* mem            = mem_->get_data().begin() + index_;
    std::ranges::copy(registers, mem);
}

void Cpu::exec_ld_vx_i()
{
    const auto x   = get_x();
    const auto mem = mem_->get_data() | std::ranges::views::drop(index_) |
                     std::ranges::views::take(x + 1);
    auto* registers = registers_.begin();
    std::ranges::copy(mem, registers);
}

} // namespace chip8
