#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/call.h>
#include <compiler/parser/expr.h>

namespace xi
{

inline const auto unwarp_unop = [](const Xi_Expr &expr
                                ) -> std::pair<Xi_Op, Xi_Expr>
{
    return std::visit(
        []<typename T>(const T &expr_) -> std::pair<Xi_Op, Xi_Expr>
        {
            if constexpr (std::same_as<T, recursive_wrapper<Xi_Unop>>)
            {
                return {expr_.get().op, expr_.get().expr};
            }
            return {Xi_Op::Add, expr_};
        },
        expr
    );
};

inline const auto binop_fold = [](const Xi_Expr &lhs, const Xi_Expr &rhs)
{
    auto [lhs_op, lhs_expr] = unwarp_unop(lhs);
    auto [rhs_op, rhs_expr] = unwarp_unop(rhs);
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

inline const auto Xi_binop_fold_go = [](Xi_Expr lhs, Xi_Expr rhs)
{
    auto [_, expr_] = unwarp_unop(binop_fold(lhs, rhs));
    return unit(expr_);
};

inline const auto combine_to_unop = [](auto C1, auto C2)
{
    return C1 >> [C2](Xi_Op op)
    {
        return C2 >> [op](Xi_Expr expr)
        {
            return unit(Xi_Expr{
                Xi_Unop{
                    .expr = expr,
                    .op   = op,
                },
            });
        };
    };
};

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

inline const auto Xi_parenmathexpr = token(s_lparen) > Xi_mathexpr >>
                                     [](auto expr)
{
    return token(s_rparen) > unit(expr);
};

inline auto Xi_factor(std::string_view input) -> Parsed_t<Xi_Expr>
{
    return (Xi_parenmathexpr || Xi_number || Xi_call || Xi_idenexpr)(input);
}

inline const Parser auto Xi_term = Xi_factor >> [](Xi_Expr lhs)
{
    return (some(combine_to_unop(Xi_mul || Xi_divide, Xi_factor), binop_fold) >>
            [lhs](Xi_Expr rhs) { return Xi_binop_fold_go(lhs, rhs); }) ||
           unit(lhs);
};

inline auto Xi_arithmeticexpr(std::string_view input) -> Parsed_t<Xi_Expr>
{
    return (
        Xi_term >>
        [](Xi_Expr lhs)
        {
            return some(
                       combine_to_unop(Xi_add || Xi_minus, Xi_term), binop_fold
                   ) >> [lhs](Xi_Expr rhs)
            {
                return Xi_binop_fold_go(lhs, rhs);
            } || unit(lhs);
        }
    )(input);
}

// <boolexpr> ::= <boolterm> | <boolterm> "||" <boolterm>
// <boolterm> ::= <boolfactor> | <boolfactor> "&&" <boolfactor>
// <boolfactor> ::= <boolvalue> | "!" <boolvalue>
// <boolvalue> ::= <boolean> | <mathexpr> <cmpop> <mathexpr> | "(" <boolexpr>
// ")"

inline const Parser auto Xi_true = token(str("true")) >> [](auto)
{
    return unit(Xi_Expr{Xi_Boolean{true}});
};

inline const Parser auto Xi_false = token(str("false")) >> [](auto)
{
    return unit(Xi_Expr{Xi_Boolean{false}});
};

inline const Parser auto Xi_boolean = Xi_true || Xi_false;

inline const Parser auto Xi_cmp = Xi_arithmeticexpr || Xi_boolean;

inline const Parser auto Xi_relation = Xi_cmp >> [](Xi_Expr lhs)
{
    return (some(
                combine_to_unop(Xi_lt || Xi_gt || Xi_le || Xi_ge, Xi_cmp),
                binop_fold
            ) >>
            [lhs](auto rhs) { return Xi_binop_fold_go(lhs, rhs); }) ||
           unit(lhs);
};

inline const Parser auto Xi_equality = Xi_relation >> [](Xi_Expr lhs)
{
    return (some(combine_to_unop(Xi_eq || Xi_ne, Xi_relation), binop_fold) >>
            [lhs](Xi_Expr rhs) { return Xi_binop_fold_go(lhs, rhs); }) ||
           unit(lhs);
};

inline const auto Xi_bool_conjunction = Xi_equality >> [](Xi_Expr lhs)
{
    return (some(combine_to_unop(Xi_and, Xi_equality), binop_fold) >>
            [lhs](Xi_Expr rhs) { return Xi_binop_fold_go(lhs, rhs); }) ||
           unit(lhs);
};

inline auto Xi_mathexpr(std::string_view input) -> Parsed_t<Xi_Expr>
{
    return (
        Xi_bool_conjunction >>
        [](Xi_Expr lhs)
        {
            return some(
                       combine_to_unop(Xi_or, Xi_bool_conjunction), binop_fold
                   ) >> [lhs](Xi_Expr rhs)
            {
                return Xi_binop_fold_go(lhs, rhs);
            } || unit(lhs);
        }
    )(input);
}
} // namespace xi
