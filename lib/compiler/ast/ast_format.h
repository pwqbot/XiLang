#pragma once

#include <compiler/ast/ast.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <range/v3/action.hpp>
#include <range/v3/view.hpp>
#include <variant>

// TODO(ding.wang): bad permance, use view to improve
constexpr auto wd =
    ranges::actions::split('\n') |
    ranges::actions::transform(
        [](auto &&line)
        { return fmt::vformat("{:\t>1}{}\n", fmt::make_format_args("", line)); }
    ) |
    ranges::actions::join | ranges::actions::reverse |
    ranges::actions::drop(1) | ranges::actions::reverse;

// format Xi_Integer
template <>
struct fmt::formatter<xi::Xi_Integer> : fmt::formatter<int>
{
    template <typename FormatContext>
    auto format(const xi::Xi_Integer &i, FormatContext &ctx)
    {
        return fmt::format_to(ctx.out(), "Xi_Integer {}", i.value);
    }
};

// format Xi_Real
template <>
struct fmt::formatter<xi::Xi_Real> : fmt::formatter<double>
{
    template <typename FormatContext>
    auto format(const xi::Xi_Real &r, FormatContext &ctx)
    {
        return fmt::format_to(ctx.out(), "Xi_Real {}", r.value);
    }
};

template <>
struct fmt::formatter<xi::Xi_Boolean> : fmt::formatter<bool>
{
    template <typename FormatContext>
    auto format(const xi::Xi_Boolean &b, FormatContext &ctx)
    {
        return fmt::format_to(ctx.out(), "Xi_Boolean {}", b.value);
    }
};

template <>
struct fmt::formatter<xi::Xi_String> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const xi::Xi_String &s, FormatContext &ctx)
    {
        auto out = ctx.out();
        out      = fmt::format_to(out, "Xi_String \"{}\"", s.value);
        return out;
    }
};

template <>
struct fmt::formatter<xi::Xi_Expr> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(xi::Xi_Expr c, FormatContext &ctx) const -> decltype(ctx.out())
    {
        return std::visit(
            [&](auto &&arg) -> decltype(auto)
            {
                const auto expr = fmt::format("{}", arg);
                return fmt::formatter<std::string>::format(expr, ctx);
            },
            c
        );
    }
};

template <>
struct fmt::formatter<xi::Xi_Binop> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const xi::Xi_Binop &b, FormatContext &ctx)
    {
        const auto lhs = fmt::format("{}", b.lhs) | wd;
        const auto rhs = fmt::format("{}", b.rhs) | wd;
        return fmt::format_to(
            ctx.out(),
            "Xi_Binop {}\n"
            "{}\n"
            "{}",
            xi::Xi_Op_To_OpStr(b.op),
            lhs,
            rhs
        );
    }
};

template <>
struct fmt::formatter<xi::Xi_Unop> : fmt::formatter<xi::Xi_Expr>
{
    template <typename FormatContext>
    auto format(const xi::Xi_Unop &u, FormatContext &ctx)
    {
        const auto expr = fmt::format("{}", u.expr) | wd;
        return fmt::format_to(
            ctx.out(), "Xi_Unop {}\n{}", xi::Xi_Op_To_OpStr(u.op), expr
        );
    }
};

template <>
struct fmt::formatter<xi::Xi_If> : fmt::formatter<xi::Xi_Expr>
{
    template <typename FormatContext>
    auto format(const xi::Xi_If &i, FormatContext &ctx)
    {
        const auto cond = fmt::format("{}", i.cond) | wd;
        const auto then = fmt::format("{}", i.then) | wd;
        const auto els  = fmt::format("{}", i.els) | wd;
        return fmt::format_to(
            ctx.out(),
            "Xi_If\n"
            "{}\n"
            "{}\n"
            "{}",
            cond,
            then,
            els
        );
    }
};

template <>
struct fmt::formatter<xi::Xi_Lam> : fmt::formatter<xi::Xi_Expr>
{
    template <typename FormatContext>
    auto format(const xi::Xi_Lam &l, FormatContext &ctx)
    {

        auto args =
            l.args |
            ranges::views::transform([](auto &&arg)
                                     { return fmt::format("{}, ", arg); }) |
            ranges::actions::join; // i don't know why can't drop join view
        // join(", ") will add new line
        args = args | ranges::views::reverse | ranges::views::drop(2) |
               ranges::views::reverse | ranges::to<std::string>;
        const auto body = fmt::format("B {}", l.body) | wd;
        return fmt::format_to(
            ctx.out(),
            "Xi_Lam\n"
            "\tA {}\n"
            "{}",
            args,
            body
        );
    }
};

template <>
struct fmt::formatter<xi::Xi_Iden> : fmt::formatter<xi::Xi_Expr>
{
    template <typename FormatContext>
    auto format(const xi::Xi_Iden &i, FormatContext &ctx)
    {
        return fmt::format_to(ctx.out(), "Xi_Iden {}", i.name);
    }
};

template <>
struct fmt::formatter<xi::Xi_Call> : fmt::formatter<xi::Xi_Expr>
{
    template <typename FormatContext>
    auto format(const xi::Xi_Call &i, FormatContext &ctx)
    {
        const auto name = fmt::format("{}", i.name) | wd;
        const auto args = []()
        {
            std::string args_;
            for (auto &arg : args_)
            {
                args_ += fmt::format("{}\n", arg) | wd;
            }
            return args_;
        };
        return fmt::format_to(
            ctx.out(),
            "Xi_Call {}\n"
            "{}\n",
            name,
            args
        );
    }
};

template <>
struct fmt::formatter<xi::Xi_Xi> : fmt::formatter<xi::Xi_Expr>
{
    template <typename FormatContext>
    auto format(const xi::Xi_Xi &i, FormatContext &ctx)
    {
        const auto name = fmt::format("{}", i.name) | wd;
        const auto expr = fmt::format("{}", i.expr) | wd;
        return fmt::format_to(
            ctx.out(),
            "Xi_Xi\n"
            "{}\n"
            "{}",
            name,
            expr
        );
    }
};

template <>
struct fmt::formatter<xi::Xi_Stmt> : fmt::formatter<xi::Xi_Expr>
{
    template <typename FormatContext>
    auto format(const xi::Xi_Stmt &i, FormatContext &ctx)
    {
        return std::visit(
            [&ctx](auto &&stmt)
            { return fmt::format_to(ctx.out(), "Xi_Stmt {}", stmt); },
            i
        );
    }
};

template <>
struct fmt::formatter<xi::Xi_Program> : fmt::formatter<xi::Xi_Expr>
{
    template <typename FormatContext>
    auto format(const xi::Xi_Program &i, FormatContext &ctx)
    {
        auto out = ctx.out();
        fmt::format_to(out, "Xi_Program qwq\n");
        for (auto &&stmt : i.stmts)
        {
            auto indent_stmt = fmt::format("{}", stmt) | wd;
            fmt::format_to(out, "{}\n", indent_stmt);
        }
        return out;
    }
};
