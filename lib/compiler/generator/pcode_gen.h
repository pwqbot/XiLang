#include "compiler/ast/all.h"
#include "compiler/ast/type.h"
#include "compiler/generator/error.h"
#include "compiler/generator/pcode.h"

#include <cstdint>

namespace xi
{

struct PCodeGenState
{
    std::vector<PCode>   pcodes;
    int64_t              cur_level;
    std::vector<int64_t> cur_base;
    struct var
    {
        std::string name;
        int64_t     level;
        int64_t     addr;
    };
    std::stack<std::map<std::string, var>> var_table;
    std::stack<int64_t>                    cur_offset;
    std::stack<std::stack<int64_t>>        old_offset;
    auto                                   Emit(Op op, int64_t l, int64_t a)
    {
        return pcodes.push_back(PCode{op, l, a});
    }
    [[nodiscard]] auto NextLabel() const -> uint64_t { return pcodes.size(); }
    [[nodiscard]] auto GetCurLevel() const -> int64_t { return cur_level; }
    auto               EnterNextLevel()
    {
        cur_level++;
        old_offset.emplace(cur_offset);
    }
    auto LeaveCurLevel()
    {
        cur_level--;
        cur_offset = old_offset.top();
        old_offset.pop();
    }

    auto EnterBlock()
    {
        var_table.emplace(var_table.top());
        cur_offset.emplace(cur_offset.top());
    }
    auto LeaveBlock()
    {
        var_table.pop();
        cur_offset.pop();
    }

    auto EnterVar(const std::string &name)
    {
        auto &vt = var_table.top();
        vt.insert_or_assign(name, var{name, cur_level, cur_offset.top()});
        cur_offset.top()++;
    }

    auto LookupVar(const std::string &name) -> var &
    {
        auto &vt = var_table.top();
        if (vt.contains(name))
        {
            return vt[name];
        }
        throw std::runtime_error("Variable not found");
    }
};

void PCodeGen(Xi_Expr, PCodeGenState);
void PCodeGen(Xi_Stmt, PCodeGenState);
void PCodeGen(std::vector<Xi_Stmt>, PCodeGenState);

auto PCodeGen(Xi_Integer i, PCodeGenState &st)
{
    st.Emit(Op::lit, 0, i.value);
}

auto PCodeGen(Xi_Real, PCodeGenState &) {}

auto PCodeGen(Xi_String, PCodeGenState &) {}

auto PCodeGen(Xi_Boolean b, PCodeGenState &st)
{
    st.Emit(Op::lit, 0, static_cast<int64_t>(b.value));
}

auto PCodeGen(Xi_Binop binop, PCodeGenState &st)
{
    PCodeGen(binop.lhs, st);
    PCodeGen(binop.rhs, st);
    switch (binop.op)
    {
    case Xi_Op::Add:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Add));
    case Xi_Op::Sub:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Sub));
    case Xi_Op::Mul:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Mul));
    case Xi_Op::Div:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Div));
    case Xi_Op::Mod:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Mod));
    case Xi_Op::Eq:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Eq));
    case Xi_Op::Neq:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Neq));
    case Xi_Op::Lt:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Lt));
    case Xi_Op::Gt:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Gt));
    case Xi_Op::Leq:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Leq));
    case Xi_Op::Geq:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Geq));
    case Xi_Op::And:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::And));
    case Xi_Op::Or:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Or));
    case Xi_Op::Xor:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Xor));
    default:
        throw std::runtime_error("unreachable");
    }
}

auto PCodeGen(Xi_Unop unop, PCodeGenState &st)
{
    PCodeGen(unop.expr, st);
    switch (unop.op)
    {
    case Xi_Op::Add:
        return;
    case Xi_Op::Sub:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Neg));
    case Xi_Op::Not:
        return st.Emit(Op::opr, 0, static_cast<int64_t>(Opr::Not));
    default:
        throw std::runtime_error("unreachable");
    }
}

auto PCodeGen(Xi_If_stmt if_stmt, PCodeGenState &st)
{
    st.EnterBlock();

    PCodeGen(if_stmt.cond, st);
    auto jpc_label = st.NextLabel();
    st.Emit(Op::jpc, 0, 0);

    PCodeGen(if_stmt.then, st);
    auto then_jmp_label = st.NextLabel();
    st.Emit(Op::jmp, 0, 0);

    st.pcodes[jpc_label].a = static_cast<int64_t>(st.NextLabel());
    PCodeGen(if_stmt.els, st);
    st.pcodes[then_jmp_label].a = static_cast<int64_t>(st.NextLabel());

    st.LeaveBlock();
}

auto PCodeGen(Xi_While while_stmt, PCodeGenState &st)
{
    st.EnterBlock();

    auto cond_label = st.NextLabel();
    PCodeGen(while_stmt.cond, st);
    auto jpc_label = st.NextLabel();
    st.Emit(Op::jpc, 0, 0);

    PCodeGen(while_stmt.body, st);
    st.Emit(Op::jmp, 0, static_cast<int64_t>(cond_label));
    st.pcodes[jpc_label].a = static_cast<int64_t>(st.NextLabel());

    st.LeaveBlock();
}

// helper type for the visitor #4
template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
// i64,
// real,
// string,
// buer,
// vararg,
// recursive_wrapper<array>,
// recursive_wrapper<function>,
// recursive_wrapper<set>,
// recursive_wrapper<types>,
auto size(type::Xi_Type t) -> int64_t
{
    return std::visit(
        overloaded{
            [](type::i64)
            {
                return 1;
            },
            [](type::real)
            {
                return 1;
            },
            [](type::string)
            {
                return 1;
            },
            [](type::buer)
            {
                return 1;
            },
            [](type::vararg)
            {
                return 1;
            },
            [](recursive_wrapper<type::array>)
            {
                return 1;
            },
            [](recursive_wrapper<type::function>)
            {
                return 1;
            },
            [](recursive_wrapper<type::set> st)
            {
                auto ans = 0;
                for (auto &&[name, t_] : st.get().members)
                {
                    ans += size(t_);
                }
                return ans;
            },
            [](recursive_wrapper<type::types>)
            {
                return 1;
            },
            [](type::unknown)
            {
                return 1;
            }},
        t
    );
}

auto PCodeGen(Xi_Var var, PCodeGenState &st)
{
    auto var_size = size(var.type);
    st.Emit(Op::ini, 0, var_size);
    st.EnterVar(var.name);
}

auto PCodeGen(Xi_Assign assign, PCodeGenState &st)
{
    PCodeGen(assign.expr, st);
    auto var = st.LookupVar(assign.name);
    st.Emit(Op::sto, st.cur_level - var.level, var.addr);
}

} // namespace xi
