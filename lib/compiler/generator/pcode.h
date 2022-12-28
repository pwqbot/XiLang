
#include <cstdint>
#include <magic_enum.hpp>

namespace xi
{

enum class Op
{
    lit,
    opr,
    lod,
    sto,
    cal,
    ini,
    jmp,
    jpc,
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
    int64_t stack_[10000];
    int64_t st_top_;
    int64_t nx_code_;
    int64_t cur_base_;

    auto interpretOpr(PCode code)
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

    auto Interpret(PCode code)
    {
        switch (code.op)
        {
        case Op::lit:
            st_top_++;
            stack_[st_top_] = code.a;
            break;
        case Op::opr:
            return interpretOpr(code);
        case Op::lod:
            st_top_++;
            stack_[st_top_] = stack_[base(code.l) + code.a];
            return;
        case Op::sto:
            stack_[base(code.l) + code.a] = stack_[st_top_];
            st_top_--;
            return;
        case Op::cal:
            break;
        case Op::ini:
            st_top_ = st_top_ + code.a;
            return;
        case Op::jmp:
            nx_code_ = code.a;
            return;
        case Op::jpc:
            if (stack_[st_top_] == code.l)
            {
                nx_code_ = code.a;
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

} // namespace xi
