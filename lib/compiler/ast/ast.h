#pragma once

#include <compare>
#include <compiler/parser/utils.h>
#include <fmt/format.h>
#include <magic_enum.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/view.hpp>
#include <string>
#include <type_traits>
#include <variant>

namespace xi
{
// A trait to check that T is one of 'Types...'
template <typename T, typename... Types>
concept is_one_of =
    std::disjunction_v<std::is_same<std::remove_cv_t<T>, Types>...>;

template <typename T, typename... Types>
concept is_one_of_recursive = std::disjunction_v<
    std::is_same<recursive_wrapper<std::remove_cv_t<T>>, Types>...>;

template <typename... Types, typename T>
auto operator==(const std::variant<Types...> &v, const T &t) noexcept
    requires is_one_of<T, Types...> || is_one_of_recursive<T, Types...>
{
    if constexpr (is_one_of_recursive<T, Types...>)
    {
        return std::get<recursive_wrapper<std::remove_cv_t<T>>>(v) == t;
    }
    else
    {
        return std::get<T>(v) == t;
    }
}

struct Xi_Boolean
{
    bool value;
    auto operator<=>(const Xi_Boolean &) const = default;
};

struct Xi_Integer
{
    int64_t value;
    auto    operator<=>(const Xi_Integer &) const = default;
};

struct Xi_Real
{
    double value;
    auto   operator<=>(const Xi_Real &) const = default;
};

struct Xi_String
{
    std::string value;
    auto        operator<=>(const Xi_String &) const = default;
};

using Xi_Literal = std::variant<Xi_Boolean, Xi_Integer, Xi_Real, Xi_String>;

enum class Xi_Op
{
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Pow,
    Eq,
    Neq,
    Lt,
    Gt,
    Leq,
    Geq,
    And,
    Or,
    Not,
};

constexpr auto OpMaps = std::
    array<std::pair<std::string_view, Xi_Op>, magic_enum::enum_count<Xi_Op>()>{{
        {"+", Xi_Op::Add},
        {"-", Xi_Op::Sub},
        {"*", Xi_Op::Mul},
        {"/", Xi_Op::Div},
        {"%", Xi_Op::Mod},
        {"^", Xi_Op::Pow},
        {"==", Xi_Op::Eq},
        {"!=", Xi_Op::Neq},
        {"<", Xi_Op::Lt},
        {">", Xi_Op::Gt},
        {"<=", Xi_Op::Leq},
        {">=", Xi_Op::Geq},
        {"&&", Xi_Op::And},
        {"||", Xi_Op::Or},
        {"!", Xi_Op::Not},
    }};

constexpr auto KeyWords = std::array<std::string_view, 6>{
    "if", "then", "else", "true", "false", "xi"};

constexpr auto IsKeyWords(std::string_view str) -> bool
{
    return ranges::find_if(KeyWords, [&](auto kw) { return kw == str; }) !=
           KeyWords.end();
}

constexpr auto OpStr_To_Xi_Op(std::string_view s) -> Xi_Op
{
    return ranges::find_if(
               OpMaps, [&](auto p) { return p.first == s; }
    )->second;
}

constexpr auto Xi_Op_To_OpStr(Xi_Op op) -> std::string_view
{
    return ranges::find_if(
               OpMaps, [&](auto p) { return p.second == op; }
    )->first;
}

enum class Xi_Type
{
    i64,
    real,
    array,
    string,
    _vararg,
    _set,
    _unknown,
};

struct Xi_Iden
{
    std::string name;
    Xi_Type     type = Xi_Type::_unknown;
    // Xi_Expr     expr;
         operator std::string() const { return name; }
    auto operator<=>(const Xi_Iden &) const = default;
};

struct Xi_Set
{
    std::string          name;
    std::vector<Xi_Iden> members;
    auto                 operator<=>(const Xi_Set &) const = default;
};

struct Xi_Decl
{
    Xi_Iden              name;
    Xi_Type              return_type;
    std::vector<Xi_Type> params_type;
    bool                 is_vararg;
    auto                 operator<=>(const Xi_Decl &) const = default;
};

struct Xi_Binop;
auto operator<=>(const Xi_Binop &lhs, const Xi_Binop &rhs)
    -> std::partial_ordering;

struct Xi_If;
auto operator<=>(const Xi_If &lhs, const Xi_If &rhs) -> std::partial_ordering;

struct Xi_Unop;
auto operator<=>(const Xi_Unop &lhs, const Xi_Unop &rhs)
    -> std::partial_ordering;

struct Xi_Lam;
auto operator<=>(const Xi_Lam &lhs, const Xi_Lam &rhs) -> std::partial_ordering;

struct Xi_Call;
auto operator<=>(const Xi_Call &lhs, const Xi_Call &rhs)
    -> std::partial_ordering;

using Xi_Expr = std::variant<
    std::monostate,
    Xi_Integer,
    Xi_Boolean,
    Xi_Real,
    Xi_String,
    Xi_Iden,
    recursive_wrapper<Xi_Unop>,
    recursive_wrapper<Xi_Binop>,
    recursive_wrapper<Xi_If>,
    recursive_wrapper<Xi_Lam>,
    recursive_wrapper<Xi_Call>>;

struct Xi_Func
{
    Xi_Iden              name;
    std::vector<Xi_Iden> params;
    Xi_Expr              expr;
    auto                 operator<=>(const Xi_Func &rhs) const = default;
};

using Xi_Stmt = std::variant<Xi_Expr, Xi_Func, Xi_Decl>;

struct Xi_Program
{
    std::vector<Xi_Stmt> stmts;
    auto                 operator<=>(const Xi_Program &rhs) const = default;
};

// binary expression
struct Xi_Binop
{
    Xi_Expr lhs;
    Xi_Expr rhs;
    Xi_Op   op;
    auto    operator==(const Xi_Binop &b) const -> bool
    {
        return *this <=> b == nullptr;
    }
};

inline auto operator<=>(const Xi_Binop &lhs, const Xi_Binop &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.lhs <=> rhs.lhs; cmp != nullptr)
    {
        return cmp;
    }
    if (auto cmp = lhs.rhs <=> rhs.rhs; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.op <=> rhs.op;
}

struct Xi_Unop
{
    Xi_Expr expr;
    Xi_Op   op;
};

inline auto operator<=>(const Xi_Unop &lhs, const Xi_Unop &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.expr <=> rhs.expr; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.op <=> rhs.op;
}

struct Xi_If
{
    Xi_Expr cond;
    Xi_Expr then;
    Xi_Expr els;
};

inline auto operator<=>(const Xi_If &lhs, const Xi_If &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.cond <=> rhs.cond; cmp != nullptr)
    {
        return cmp;
    }
    if (auto cmp = lhs.then <=> rhs.then; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.els <=> rhs.els;
}

using Xi_Args = std::vector<Xi_Iden>;

struct Xi_Lam
{
    std::vector<Xi_Iden> args;
    Xi_Expr              body;
};

inline auto operator<=>(const Xi_Lam &lhs, const Xi_Lam &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.args <=> rhs.args; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.body <=> rhs.body;
}

struct Xi_Call
{
    Xi_Iden              name;
    std::vector<Xi_Expr> args;
};

inline auto operator<=>(const Xi_Call &lhs, const Xi_Call &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.args <=> rhs.args; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.name <=> rhs.name;
}
} // namespace xi
