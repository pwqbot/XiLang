#pragma once

#include <compiler/ast/enum_format.h>
#include <cstdint>
#include <fmt/std.h>
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>
#include <vector>

namespace xi
{

enum class Op
{
    lit,
    opr,
    lod,
    sto,
    cal,
    cap,
    ini,
    jmp,
    jpc,
    ret,
};

struct PCode
{
    Op      op;
    int64_t l;
    int64_t a;
};

enum class Opr
{
    Call,
    Neg,
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Odd,
    Eq,
    Neq,
    Lt,
    Gt,
    Leq,
    Geq,
    And,
    Or,
    Not,
    Xor,
};

struct PCodeM
{
    // auto Interpret(std::vector<PCode> codes) {
    //
    //     }
    int64_t stack_[10000] = {};
    int64_t st_top_       = 0;
    int64_t nx_code_      = 0;
    int64_t cur_base_     = 1;
    int64_t nx_base_      = 0;
    int64_t rt_code_      = 0;
    int64_t old_top_      = 0;
    bool    fetch_next_   = true;

    void interpretOpr(PCode code)
    {
        auto opr = static_cast<Opr>(code.a);
        switch (opr)
        {
        case Opr::Call:
            st_top_   = cur_base_ - 1;
            nx_code_  = stack_[st_top_ + 3];
            cur_base_ = stack_[st_top_ + 2];
            return;
        case Opr::Neg:
            stack_[st_top_] = -stack_[st_top_];
            return;
        case Opr::Add:
            st_top_--;
            stack_[st_top_] += stack_[st_top_ + 1];
            return;
        case Opr::Sub:
            st_top_--;
            stack_[st_top_] -= stack_[st_top_ + 1];
            return;
        case Opr::Mul:
            st_top_--;
            stack_[st_top_] *= stack_[st_top_ + 1];
            return;
        case Opr::Div:
            st_top_--;
            stack_[st_top_] /= stack_[st_top_ + 1];
            return;
        case Opr::Odd:
            stack_[st_top_] %= 2;
            return;
        case Opr::Eq:
            st_top_--;
            stack_[st_top_] = stack_[st_top_] == stack_[st_top_ + 1];
            return;
        case Opr::Neq:
            st_top_--;
            stack_[st_top_] = stack_[st_top_] != stack_[st_top_ + 1];
            return;
        case Opr::Lt:
            st_top_--;
            stack_[st_top_] = stack_[st_top_] < stack_[st_top_ + 1];
            return;
        case Opr::Geq:
            st_top_--;
            stack_[st_top_] = stack_[st_top_] >= stack_[st_top_ + 1];
            return;
        case Opr::Gt:
            st_top_--;
            stack_[st_top_] = stack_[st_top_] > stack_[st_top_ + 1];
            return;
        case Opr::Leq:
            st_top_--;
            stack_[st_top_] = stack_[st_top_] <= stack_[st_top_ + 1];
            return;
        case Opr::Mod:
            st_top_--;
            stack_[st_top_] %= stack_[st_top_ + 1];
            return;
        case Opr::And:
            st_top_--;
            stack_[st_top_] = stack_[st_top_] && stack_[st_top_ + 1];
            return;
        case Opr::Or:
            st_top_--;
            stack_[st_top_] = stack_[st_top_] || stack_[st_top_ + 1];
            return;
        case Opr::Not:
            stack_[st_top_] = !stack_[st_top_];
            return;
        case Opr::Xor:
            st_top_--;
            stack_[st_top_] = stack_[st_top_] ^ stack_[st_top_ + 1];
            return;
        }
    }

    auto outputStack() -> std::string
    {
        std::string res;
        for (int i = 0; i <= st_top_; i++)
        {
            res += fmt::format("{} ", stack_[i]);
        }
        return res;
    }

    void Interpret(std::vector<PCode> codes, int64_t begin)
    {
        nx_code_ = begin;
        do
        {
            spdlog::info(
                "code: {} {} {}",
                codes[(size_t)nx_code_].op,
                codes[(size_t)nx_code_].l,
                codes[(size_t)nx_code_].a
            );
            Interpret(codes[static_cast<uint64_t>(nx_code_)]);
            spdlog::info("st_top_ {}, stack: {}", st_top_, outputStack());
            if (fetch_next_)
            {
                nx_code_++;
            }
        } while (nx_code_ != begin + 2);
    }

    void Interpret(PCode code)
    {
        switch (code.op)
        {
        case Op::lit:
            spdlog::info("lit {}", code.a);
            st_top_++;
            stack_[st_top_] = code.a;
            fetch_next_     = true;
            break;
        case Op::opr:
            fetch_next_ = true;
            return interpretOpr(code);
        case Op::lod:
            st_top_++;
            stack_[st_top_] = stack_[base(code.l) + code.a];
            fetch_next_     = true;
            return;
        case Op::sto:
            fetch_next_                   = true;
            stack_[base(code.l) + code.a] = stack_[st_top_];
            st_top_--;
            return;
        case Op::cap:
            fetch_next_         = true;
            stack_[st_top_ + 1] = base(code.l);
            stack_[st_top_ + 2] = cur_base_;
            nx_base_            = st_top_ + 1;
            st_top_ += 3;
            break;
        case Op::cal:
            fetch_next_           = false;
            stack_[cur_base_ + 2] = nx_code_;
            cur_base_             = nx_base_;
            nx_code_              = code.a;
            break;
        case Op::ret:
            fetch_next_ = true;
            old_top_    = st_top_;
            st_top_     = cur_base_ - 1;
            nx_code_    = stack_[st_top_ + 3];
            cur_base_   = stack_[st_top_ + 2];
            std::copy(
                stack_ + old_top_ - code.a + 1,
                stack_ + old_top_,
                stack_ + st_top_ + 1
            );
            break;
        case Op::ini:
            fetch_next_ = true;
            st_top_     = st_top_ + code.a;
            return;
        case Op::jmp:
            fetch_next_ = false;
            nx_code_    = code.a;
            return;
        case Op::jpc:
            if (stack_[st_top_] == code.l)
            {
                fetch_next_ = false;
                nx_code_    = code.a;
            }
            else
            {
                fetch_next_ = true;
            }
            st_top_--;
            return;
        }
    }

    auto base(int64_t l) -> int64_t
    {
        int64_t base = cur_base_;
        while (l > 0)
        {
            base = stack_[base];
            l--;
        }
        return base;
    }
};

auto pCodeToString(std::vector<PCode> codes)
{
    int         i = 0;
    std::string res;
    for (auto code : codes)
    {
        res += fmt::format("{}  {}  {}  {}\n", i, code.op, code.l, code.a);
        i++;
    }
    return res;
}

void Interpret(std::vector<PCode> codes)
{
    PCodeM m;
    m.Interpret(codes, static_cast<int>(codes.size()) - 2);
}

} // namespace xi
