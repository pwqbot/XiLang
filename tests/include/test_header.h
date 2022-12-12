#pragma once

// clang-format off
#include <compiler/ast/ast_format.h>
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
    std::same_as<T, xi::Xi_Lam>;

template <typename T>
    requires is_one_of_ast_node<T>
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

template <typename T, typename U>
struct AstNodeMatcher : Catch::Matchers::MatcherGenericBase
{
    T expected_first_;
    U expected_second_;

    explicit AstNodeMatcher(T expected_first, U expected_second) :
        expected_first_{expected_first}, expected_second_{expected_second}
    {
    }

    template <typename V>
    auto match(V v) const -> bool
    {
        auto [first, second] = v.value();
        return first == expected_first_ && second == expected_second_;
    }

    auto describe() const -> std::string override
    {
        return "\n==\n" + fmt::format("{}", expected_first_);
    }
};

template <typename T>
struct FormatMatcher : Catch::Matchers::MatcherGenericBase
{
    T expected;

    explicit FormatMatcher(T expected) : expected{expected} {}

    template <typename V>
    auto match(V v) const -> bool
    {
        return fmt::format("{}", v) == expected;
    }

    auto describe() const -> std::string override
    {
        return fmt::format("is equal to {}", expected);
    }
};
