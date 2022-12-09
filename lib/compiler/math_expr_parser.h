#pragma once

#include <compiler/ast.h>
#include <compiler/basic_parsers.h>

namespace xi
{
auto Xi_expr(std::string_view input) -> Parsed_t<Xi_Expr>;

// auto Xi_expr(std::string_view input) -> Parsed_t<Xi_Expr>;

// <mathexpr> ::= <term> | <mathexpr> "+-" <term>
// <term> ::= <factor> | <term> "*/" <factor>
// <factor> ::= "(" <mathexpr> ")" | <number> | <iden>
// <number> ::=

// and we need to resovle left recursion
// <mathexpr> ::= <term> ("+-" <term>)*
// <term> ::= <factor> ("*/" <factor>)*
// <factor> ::= "(" <mathexpr> ")" | <number>
auto Xi_mathexpr(std::string_view input) -> Parsed_t<Xi_Expr>;

inline const Parser auto s_natural = some(s_digit);

inline const Parser auto Xi_integer = token(maybe(symbol('-'))) >> [](auto x)
{
    return token(s_natural) >> [x](auto nat)
    {
        if (x)
        {
            return unit(Xi_Expr{Xi_Integer{.value{-std::stoi(nat)}}});
        }
        return unit(Xi_Expr{Xi_Integer{.value{std::stoi(nat)}}});
    };
};

// real = integer "." integer
inline const Parser auto Xi_real = Xi_integer >> [](const Xi_Integer &integer)
{
    return token(s_dot) > token(s_natural) >> [integer](auto nat)
    {
        return unit(Xi_Expr{Xi_Real{
            .value{std::stod(std::to_string(integer.value) + "." + nat)}}});
    };
};

inline const Parser auto Xi_number = Xi_real || Xi_integer;

const auto Xi_parenmathexpr = token(s_lparen) > Xi_mathexpr >> [](auto expr)
{
    return token(s_rparen) >> [expr](auto)
    {
        return unit(expr);
    };
};

inline const auto Unwarp_Unop = [](const Xi_Expr &expr
                                ) -> std::pair<Xi_Op, Xi_Expr>
{
    return std::visit(
        []<typename T>(const T &expr) -> std::pair<Xi_Op, Xi_Expr>
        {
            if constexpr (std::same_as<T, recursive_wrapper<Xi_Unop>>)
            {
                return {expr.get().op, expr.get().expr};
            }
            return {Xi_Op::Add, expr};
        },
        expr
    );
};

const auto binop_fold = [](const Xi_Expr &lhs, const Xi_Expr &rhs)
{
    auto [lhs_op, lhs_expr] = Unwarp_Unop(lhs);
    auto [rhs_op, rhs_expr] = Unwarp_Unop(rhs);
    return Xi_Expr{
        Xi_Unop{
            .expr{Xi_Binop{
                .lhs = lhs_expr,
                .rhs = rhs_expr,
                .op  = rhs_op,
            }},
            .op = lhs_op,
        },
    };
};

auto Xi_factor(std::string_view input) -> Parsed_t<Xi_Expr>
{
    return (Xi_number || Xi_iden || Xi_parenmathexpr)(input);
}

inline const Parser auto Xi_mulop = (Xi_mul || Xi_divide) >> [](auto op)
{
    return Xi_factor >> [op](auto factor)
    {
        return unit(Xi_Expr{
            Xi_Unop{
                .expr{factor},
                .op{op},
            },
        });
    };
};

inline const Parser auto Xi_term = Xi_factor >> [](const Xi_Expr &lhs)
{
    return (some(Xi_mulop, binop_fold) >>
            [lhs](const Xi_Expr &rhs)
            {
                auto [op, rhs_expr] = Unwarp_Unop(rhs);
                return unit(Xi_Expr{
                    Xi_Binop{
                        .lhs = lhs,
                        .rhs = rhs_expr,
                        .op  = op,
                    },
                });
            }) ||
           unit(lhs);
};

inline const Parser auto Xi_addop = (Xi_add || Xi_minus) >> [](auto op)
{
    return Xi_term >> [op](auto term)
    {
        return unit(Xi_Expr{Xi_Unop{
            .expr = term,
            .op   = op,
        }});
    };
};

auto Xi_mathexpr(std::string_view input) -> Parsed_t<Xi_Expr>
{
    return (
        Xi_term >>
        [](const Xi_Expr &lhs)
        {
            return (some(Xi_addop, binop_fold) >>
                    [lhs](const Xi_Expr &rhs)
                    {
                        auto [op, rhs_expr] = Unwarp_Unop(rhs);
                        return unit(Xi_Expr{
                            Xi_Binop{
                                .lhs = lhs,
                                .rhs = rhs_expr,
                                .op  = op,
                            },
                        });
                    }) ||
                   unit(lhs);
        }
    )(input);
}
} // namespace xi
