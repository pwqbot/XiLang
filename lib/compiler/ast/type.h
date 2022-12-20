#pragma once

#include <array>
#include <compiler/ast/utils.h>
#include <compiler/parser/utils.h>
#include <optional>
#include <range/v3/algorithm.hpp>
#include <variant>
#include <vector>

namespace xi
{

namespace type
{

struct i64
{
    auto operator<=>(const i64 &) const = default;
};

struct real
{
    auto operator<=>(const real &) const = default;
};

struct string
{
    auto operator<=>(const string &) const = default;
};

struct buer
{
    auto operator<=>(const buer &) const = default;
};

struct unknown
{
    auto operator<=>(const unknown &) const = default;
};

struct vararg
{
    auto operator<=>(const vararg &) const = default;
};

struct array;
auto operator<=>(const array &lhs, const array &rhs) -> std::partial_ordering;

struct function;
auto operator<=>(const function &lhs, const function &rhs)
    -> std::partial_ordering;

struct set;
auto operator<=>(const set &lhs, const set &rhs) -> std::partial_ordering;

using Xi_Type = std::variant<
    i64,
    real,
    string,
    buer,
    vararg,
    recursive_wrapper<array>,
    recursive_wrapper<function>,
    recursive_wrapper<set>,
    unknown>;

struct array
{
    Xi_Type inner_type;
};

inline auto operator<=>(const array &lhs, const array &rhs)
    -> std::partial_ordering
{
    return lhs.inner_type <=> rhs.inner_type;
}

struct function
{
    Xi_Type              return_type;
    std::vector<Xi_Type> param_types;
    bool                 is_vararg = false;
};

inline auto operator<=>(const function &lhs, const function &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.return_type <=> rhs.return_type; cmp != nullptr)
    {
        return cmp;
    }
    if (auto cmp = lhs.param_types <=> rhs.param_types; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.is_vararg <=> rhs.is_vararg;
}

struct set
{
    std::string          name;
    std::vector<Xi_Type> members = {};
};

inline auto operator<=>(const set &lhs, const set &rhs) -> std::partial_ordering
{
    if (auto cmp = lhs.name <=> rhs.name; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.members <=> rhs.members;
}

inline constexpr auto BuiltinTypes =
    std::array<std::pair<std::string_view, Xi_Type>, 5>{{
        {"i64", i64{}},
        {"real", real{}},
        {"string", string{}},
        {"buer", buer{}},
        {"...", vararg{}},
    }};

inline auto ToBuiltinTypes(std::string_view s) -> std::optional<Xi_Type>
{
    const auto *it = ranges::find_if(
        BuiltinTypes,
        [&](auto &&pair)
        {
            return pair.first == s;
        }
    );
    if (it != BuiltinTypes.end())
    {
        return it->second;
    }
    return std::nullopt;
}

} // namespace type

} // namespace xi
