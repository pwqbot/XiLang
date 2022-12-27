#pragma once

#include <array>
#include <compiler/utils/recursive_wrapper.h>
#include <compiler/utils/variant_cmp.h>
#include <optional>
#include <range/v3/algorithm.hpp>
#include <variant>
#include <vector>

namespace xi::type
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

struct types;
auto operator<=>(const types &lhs, const types &rhs) -> std::partial_ordering;

using Xi_Type = std::variant<
    i64,
    real,
    string,
    buer,
    vararg,
    recursive_wrapper<array>,
    recursive_wrapper<function>,
    recursive_wrapper<set>,
    recursive_wrapper<types>,
    unknown>;

struct types
{
    std::vector<Xi_Type> types;
};

inline auto operator<=>(const types &lhs, const types &rhs)
    -> std::partial_ordering
{
    return lhs.types <=> rhs.types;
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
    std::string                                  name;
    std::vector<std::pair<std::string, Xi_Type>> members = {};
};

inline auto operator<=>(const set &lhs, const set &rhs) -> std::partial_ordering
{
    if (auto cmp = lhs.name <=> rhs.name; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.members <=> rhs.members;
}

struct array
{
    Xi_Type inner_type;
    explicit array(Xi_Type inner_type_) : inner_type(std::move(inner_type_)) {}
};

inline auto isSet(Xi_Type type)
{
    return std::holds_alternative<recursive_wrapper<set>>(type);
}

inline auto operator<=>(const array &lhs, const array &rhs)
    -> std::partial_ordering
{
    return lhs.inner_type <=> rhs.inner_type;
}

inline constexpr auto BuiltinTypes =
    std::array<std::pair<std::string_view, Xi_Type>, 6>{{
        {"i64", i64{}},
        {"int", i64{}},
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

} // namespace xi::type
