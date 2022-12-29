#include "compiler/ast/all.h"
#include "compiler/ast/type.h"
#include "compiler/generator/error.h"
#include "compiler/generator/pcode.h"

#include <spdlog/spdlog.h>

namespace xi
{

struct PCodeGenState
{
    std::vector<PCode>   pcodes;
    int64_t              cur_level{0};
    std::vector<int64_t> cur_base;
    struct var
    {
        std::string name;
        int64_t     level;
        int64_t     addr;
    };
    struct func
    {
        type::function type;
        int64_t        level;
        int64_t        addr;
    };
    std::stack<std::map<std::string, var>>  var_table;
    std::stack<std::map<std::string, func>> func_table;
    std::stack<int64_t>                     cur_offset;
    std::stack<std::stack<int64_t>>         old_offset;
    auto                                    Emit(Op op, int64_t l, int64_t a)
    {
        return pcodes.push_back(PCode{op, l, a});
    }

    PCodeGenState()
    {
        cur_base.push_back(0);
        cur_offset.push(0);
        old_offset.push(std::stack<int64_t>());
        var_table.push(std::map<std::string, var>());
        func_table.push(std::map<std::string, func>());
    }

    [[nodiscard]] auto NextLabel() const -> uint64_t { return pcodes.size(); }
    [[nodiscard]] auto GetCurLevel() const -> int64_t { return cur_level; }
    auto               EnterNextLevel(int64_t param_num)
    {
        cur_level++;
        old_offset.emplace(cur_offset);
        while (!cur_offset.empty())
        {
            cur_offset.pop();
        }
        cur_offset.push(param_num + 3);
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
        func_table.emplace(func_table.top());
        cur_offset.emplace(cur_offset.top());
    }
    auto LeaveBlock()
    {
        func_table.pop();
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

    auto
    EnterFunc(const std::string &name, const type::function &type, int64_t addr)
    {
        spdlog::info("EnterFunc: {}", name);
        auto &ft = func_table.top();
        ft.insert_or_assign(name, func{type, cur_level, addr});
    }

    auto LookupFunc(const std::string &name) -> func &
    {
        auto &ft = func_table.top();
        if (ft.contains(name))
        {
            return ft[name];
        }
        throw std::runtime_error("Function not found");
    }
};

void PCodeGen(Xi_Expr, PCodeGenState &);
void PCodeGen(Xi_Stmt, PCodeGenState &);
void PCodeGen(std::vector<Xi_Stmt>, PCodeGenState &);

template <typename T>
auto PCodeGen(recursive_wrapper<T> wrapper, PCodeGenState &st)
{
    return PCodeGen(wrapper.get(), st);
}

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
                    ans += static_cast<int>(size(t_));
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

auto PCodeGen(Xi_Return rt, PCodeGenState &st)
{
    PCodeGen(rt.expr, st);
    st.Emit(Op::ret, 0, size(rt.type));
}

auto PCodeGen(Xi_Comment /*unused*/, PCodeGenState /*unused*/) {}

void PCodeGen(Xi_Decl decl, PCodeGenState &st)
{
    return std::visit(
        [decl, &st](auto decl_type_wrapper)
        {
            if constexpr (std::same_as<
                              std::decay_t<decltype(decl_type_wrapper)>,
                              recursive_wrapper<type::function>>)
            {
                auto decl_type = decl_type_wrapper.get();
                st.EnterFunc(
                    decl.name, decl_type, static_cast<int64_t>(st.NextLabel())
                );
                return;
            }
            else
            {
                return;
            }
        },
        decl.type
    );
}

void pCodeGenExprFunc(Xi_Func, PCodeGenState &) {}

void pCodeGenStmtFunc(Xi_Func func, PCodeGenState &st)
{
    PCodeGen(func.stmts, st);
}

void PCodeGen(Xi_Func func, PCodeGenState &st)
{
    st.EnterNextLevel(static_cast<int64_t>(func.params.size()));

    auto f = st.LookupFunc(func.name);
    for (auto &&name : func.params)
    {
        st.EnterVar(name);
    }

    if (func.expr != std::monostate{})
    {
        pCodeGenExprFunc(func, st);
    }
    else
    {
        pCodeGenStmtFunc(func, st);
    }

    st.LeaveCurLevel();
}

void PCodeGen(Xi_Array, PCodeGenState &) {}

void PCodeGen(Xi_Call func, PCodeGenState &st)
{
    auto f = st.LookupFunc(func.name);
    st.Emit(Op::cap, st.cur_level - f.level, f.addr);
    for (auto &&arg : func.args)
    {
        PCodeGen(arg, st);
    }
    st.Emit(Op::cal, 0, f.addr);
}

void PCodeGen(Xi_Lam, PCodeGenState &) {}
void PCodeGen(Xi_If, PCodeGenState &) {}

void PCodeGen(Xi_Iden iden, PCodeGenState &st)
{
    auto var = st.LookupVar(iden.name);
    st.Emit(Op::lod, st.cur_level - var.level, var.addr);
}

void PCodeGen(Xi_ArrayIndex, PCodeGenState &) {}
void PCodeGen(std::monostate, PCodeGenState &) {}

void PCodeGen(Xi_Expr expr, PCodeGenState &st)
{
    return std::visit(
        [&](auto arg)
        {
            PCodeGen(arg, st);
        },
        expr
    );
}

void PCodeGen(Xi_Stmt stmt, PCodeGenState &st)
{
    return std::visit(
        [&](auto arg)
        {
            PCodeGen(arg, st);
        },
        stmt
    );
}
void PCodeGen(std::vector<Xi_Stmt> stmts, PCodeGenState &st)
{
    for (auto &&stmt : stmts)
    {
        PCodeGen(stmt, st);
    }
}

void PCodeGen(Xi_Stmts stmts, PCodeGenState &st)
{
    for (auto &&stmt : stmts.stmts)
    {
        PCodeGen(stmt, st);
    }
}

auto PCodeGen(Xi_Program program) -> ExpectedCodeGen<std::vector<PCode>>
{
    PCodeGenState st;
    PCodeGen(program.stmts, st);
    auto main_f = st.LookupFunc("main");
    st.Emit(Op::cap, 0, 0);
    st.Emit(Op::cal, 0, main_f.addr);
    int i = 0;
    for (auto &&code : st.pcodes)
    {
        spdlog::error("{} {} {} {}\n", i, code.op, code.l, code.a);
        i++;
    }
    return st.pcodes;
}

} // namespace xi
