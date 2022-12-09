#pragma once

#include <compare>
#include <compiler/utils.h>
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
    int  value;
    auto operator<=>(const Xi_Integer &) const = default;
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

constexpr auto KeyWords = std::array<std::string_view, 3>{"if", "then", "else"};

constexpr auto IsKeyWords(std::string_view str) -> bool
{
    return ranges::find_if(KeyWords, [&](auto kw) { return kw == str; }) !=
           KeyWords.end();
}

constexpr auto OpStr_To_Xi_Op(std::string_view s) -> Xi_Op
{
    return ranges::find_if(OpMaps, [&](auto p) { return p.first == s; })
        ->second;
}

constexpr auto Xi_Op_To_OpStr(Xi_Op op) -> std::string_view
{
    return ranges::find_if(OpMaps, [&](auto p) { return p.second == op; })
        ->first;
}

struct Xi_Iden
{
    std::string name;
    // Xi_Expr     expr;
    auto operator<=>(const Xi_Iden &) const = default;
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
    recursive_wrapper<Xi_Lam>>;

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

auto operator<=>(const Xi_Binop &lhs, const Xi_Binop &rhs)
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

auto operator<=>(const Xi_Unop &lhs, const Xi_Unop &rhs)
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

auto operator<=>(const Xi_If &lhs, const Xi_If &rhs) -> std::partial_ordering
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

auto operator<=>(const Xi_Lam &lhs, const Xi_Lam &rhs) -> std::partial_ordering
{
    if (auto cmp = lhs.args <=> rhs.args; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.body <=> rhs.body;
}

} // namespace xi
