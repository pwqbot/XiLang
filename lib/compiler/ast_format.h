#pragma once

#include <compiler/ast.h>
#include <fmt/format.h>

// format Xi_Integer
template <>
struct fmt::formatter<xi::Xi_Integer> : fmt::formatter<int> {
    template <typename FormatContext>
    auto format(const xi::Xi_Integer &i, FormatContext &ctx) {
        auto out = ctx.out();
        out      = fmt::format_to(out, "Xi_Integer ");
        out      = fmt::formatter<int>::format(i.value, ctx);
        return out;
    }
};

// format Xi_Real
template <>
struct fmt::formatter<xi::Xi_Real> : fmt::formatter<double> {
    template <typename FormatContext>
    auto format(const xi::Xi_Real &r, FormatContext &ctx) {
        auto out = ctx.out();
        out      = fmt::format_to(out, "Xi_Real ");
        out      = fmt::formatter<double>::format(r.value, ctx);
        return out;
    }
};

template <>
struct fmt::formatter<xi::Xi_Boolean> : fmt::formatter<bool> {
    template <typename FormatContext>
    auto format(const xi::Xi_Boolean &b, FormatContext &ctx) {
        auto out = ctx.out();
        out      = fmt::format_to(out, "Xi_Boolean ");
        out      = fmt::formatter<bool>::format(b.value, ctx);
        return out;
    }
};

template <>
struct fmt::formatter<xi::Xi_String> : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(const xi::Xi_String &s, FormatContext &ctx) {
        auto out = ctx.out();
        out      = fmt::format_to(out, "Xi_String \"");
        out      = fmt::formatter<std::string>::format(s.value, ctx);
        out      = fmt::format_to(out, "\"");
        return out;
    }
};

template <>
struct fmt::formatter<xi::Xi_Expr> : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(xi::Xi_Expr c, FormatContext &ctx) const
        -> decltype(ctx.out()) {
        return std::visit(
            [&](auto &&arg) -> decltype(auto) {
                return fmt::formatter<std::string>::format(
                    fmt::format("{}", arg), ctx);
            },
            c);
    }
};

template <>
struct fmt::formatter<xi::Xi_Binop> : fmt::formatter<xi::Xi_Expr> {
    template <typename FormatContext>
    auto format(const xi::Xi_Binop &b, FormatContext &ctx) {
        auto out = ctx.out();
        out      = fmt::format_to(out, "Xi_Binop ");
        out      = fmt::format_to(out, "{}\n", xi::Xi_Op_To_OpStr(b.op));
        out      = fmt::format_to(out, "\t");
        out      = fmt::formatter<xi::Xi_Expr>::format(b.lhs, ctx);
        out      = fmt::format_to(out, "\n\t");
        out      = fmt::formatter<xi::Xi_Expr>::format(b.rhs, ctx);
        return out;
    }
};
