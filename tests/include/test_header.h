#pragma once

// clang-format off
#include <compiler/ast/ast_format.h>
#include <compiler/ast/type_format.h>
#include <compiler/ast/type.h>
//clang-format on
#include <catch2/matchers/catch_matchers_templated.hpp>
#include <catch2/catch_test_macros.hpp>

namespace Catch
{

template <typename T>
concept is_one_of_ast_node =
    std::same_as<T, xi::Xi_Real> || std::same_as<T, xi::Xi_Expr> ||
    std::same_as<T, xi::Xi_String> || std::same_as<T, xi::Xi_Iden> ||
    std::same_as<T, xi::Xi_Binop> || std::same_as<T, xi::Xi_Unop> ||
    std::same_as<T, xi::Xi_If> || std::same_as<T, xi::Xi_Integer> ||
    std::same_as<T, xi::Xi_Lam> || 
std::same_as<T, xi::Xi_Set>;

template <typename T>
concept is_one_of_type_node =
    std::same_as<T, xi::type::i64> || std::same_as<T, xi::type::real> ||
    std::same_as<T, xi::type::string> || std::same_as<T, xi::type::buer> ||
    std::same_as<T, xi::type::function> || std::same_as<T, xi::type::set> ||
    std::same_as<T, xi::type::array> || std::same_as<T, xi::type::vararg> || std::same_as<T, xi::type::unknown>;



template <typename T>
    requires is_one_of_ast_node<T> || is_one_of_type_node<T>
struct StringMaker<T>
{
    static auto convert(const T &value) -> std::string
    {
        return fmt::format("{}", value);
    }
};

template <typename T>
struct StringMaker<std::optional<T>>
{
    static auto convert(const std::optional<T> &value) -> std::string
    {
        if (value == std::nullopt)
        {
            return "std::nullopt";
        }
        return fmt::format("{}", value.value().first);
    }
};

} // namespace Catch

template <typename T, typename U = std::string>
struct AstNodeMatcher : Catch::Matchers::MatcherGenericBase
{
    T expected_first_;
    U expected_second_;

    explicit AstNodeMatcher(T expected_first, U expected_second) :
        expected_first_{expected_first}, expected_second_{expected_second}
    {
    }
    explicit AstNodeMatcher(T expected_first) :
        expected_first_{expected_first}
    {
    }

    template <typename V>
    auto match(V v) const -> bool
    {
        if(!v.has_value())
        {
            return false;
        }
        auto [first, second] = v.value();
        return first == expected_first_ && second == expected_second_;
    }

    auto describe() const -> std::string override
    {
        return "\n==\n" + fmt::format("first: {}\nsecond: {}", expected_first_, expected_second_);
    }
};

template <typename T>
struct FormatMatcher : Catch::Matchers::MatcherGenericBase
{
    T expected_;

    explicit FormatMatcher(T expected) : expected_{expected} {}

    template <typename V>
    auto match(V v) const -> bool
    {
        return fmt::format("{}", v) == expected_;
    }

    auto describe() const -> std::string override
    {
        return fmt::format("is equal to {}", expected_);
    }
};
