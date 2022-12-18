#pragma once

#include <compiler/ast/type.h>
#include <fmt/core.h>

template <>
struct fmt::formatter<xi::type::i64> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const xi::type::i64 /*unused*/, FormatContext &ctx) const
    {
        return fmt::format_to(ctx.out(), "i64");
    }
};

template <>
struct fmt::formatter<xi::type::real> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const xi::type::real /*unused*/, FormatContext &ctx) const
    {
        return fmt::format_to(ctx.out(), "real");
    }
};

template <>
struct fmt::formatter<xi::type::string> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const xi::type::string /*unused*/, FormatContext &ctx) const
    {
        return fmt::format_to(ctx.out(), "string");
    }
};

template <>
struct fmt::formatter<xi::type::buer> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const xi::type::buer /*unused*/, FormatContext &ctx) const
    {
        return fmt::format_to(ctx.out(), "buer");
    }
};

template <>
struct fmt::formatter<xi::type::unknown> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const xi::type::unknown /*unused*/, FormatContext &ctx) const
    {
        return fmt::format_to(ctx.out(), "unknown");
    }
};

template <>
struct fmt::formatter<xi::type::vararg> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const xi::type::vararg /*unused*/, FormatContext &ctx) const
    {
        return fmt::format_to(ctx.out(), "vararg");
    }
};

template <>
struct fmt::formatter<xi::type::array> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const xi::type::array /*unused*/, FormatContext &ctx) const
    {
        return fmt::format_to(ctx.out(), "array");
    }
};

template <>
struct fmt::formatter<xi::type::function> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const xi::type::function /*unused*/, FormatContext &ctx) const
    {
        return fmt::format_to(ctx.out(), "function");
    }
};

template <>
struct fmt::formatter<xi::type::set> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const xi::type::set /*unused*/, FormatContext &ctx) const
    {
        return fmt::format_to(ctx.out(), "set");
    }
};

template <>
struct fmt::formatter<xi::type::Xi_Type> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const xi::type::Xi_Type &i, FormatContext &ctx) const
    {
        return std::visit(
            [&ctx](auto &&type)
            { return fmt::format_to(ctx.out(), "{}", type); },
            i
        );
    }
};
