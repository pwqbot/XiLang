#pragma once

#include <compiler/ast.h>
#include <compiler/ast_format.h>
#include <compiler/basic_parsers.h>
#include <iostream>

namespace xi {

const auto Xi_iden = token(some(s_alphanum || s_underscore)) >> [](auto name) {
    return unit(Xi_Expr{Xi_Iden{.name{name}}});
};

auto Xi_expr(std::string_view input) -> Parsed_t<Xi_Expr>;

// <mathexpr> ::= <term> | <mathexpr> "+-" <term>
// <term> ::= <factor> | <term> "*/" <factor>
// <factor> ::= "(" <mathexpr> ")" | <number> | <iden>
// <number> ::=

// and we need to resovle left recursion
// <mathexpr> ::= <term> ("+-" <term>)*
// <term> ::= <factor> ("*/" <factor>)*
// <factor> ::= "(" <mathexpr> ")" | <number>
auto Xi_mathexpr(std::string_view input) -> Parsed_t<Xi_Expr>;

inline const auto Unwarp_Unop =
    [](const Xi_Expr &expr) -> std::pair<Xi_Op, Xi_Expr> {
    return std::visit(
        []<typename T>(const T &expr) -> std::pair<Xi_Op, Xi_Expr> {
            if constexpr (std::same_as<T, recursive_wrapper<Xi_Unop>>) {
                return {expr.get().op, expr.get().expr};
            }
            return {Xi_Op::Add, expr};
        },
        expr);
};

const auto binop_fold = [](const Xi_Expr& lhs, const Xi_Expr& rhs) {
    auto [lhs_op, lhs_expr] = Unwarp_Unop(lhs);
    auto [rhs_op, rhs_expr] = Unwarp_Unop(rhs);
    return Xi_Expr{
        Xi_Unop{.expr{Xi_Binop{.lhs{lhs_expr}, .rhs{rhs_expr}, .op{rhs_op}}},
                .op{lhs_op}}};
};

const auto Xi_parenmathexpr = token(s_lparen) > Xi_mathexpr >> [](auto expr) {
    return token(s_rparen) >> [expr](auto) {
        return unit(expr);
    };
};

auto Xi_factor(std::string_view input) -> Parsed_t<Xi_Expr> {
    return (Xi_number || Xi_iden || Xi_parenmathexpr)(input);
}

inline const Parser auto Xi_mulop = (Xi_mul || Xi_divide) >> [](auto op) {
    return Xi_factor >> [op](auto factor) {
        return unit(Xi_Expr{Xi_Unop{.expr{factor}, .op{op}}});
    };
};

inline const Parser auto Xi_term = Xi_factor >> [](const Xi_Expr &lhs) {
    return (some(Xi_mulop, binop_fold) >>
            [lhs](const Xi_Expr &rhs) {
                auto [op, rhs_expr] = Unwarp_Unop(rhs);
                return unit(
                    Xi_Expr{Xi_Binop{.lhs{lhs}, .rhs{rhs_expr}, .op{op}}});
            }) ||
           unit(lhs);
};

inline const Parser auto Xi_addop = (Xi_add || Xi_minus) >> [](auto op) {
    return Xi_term >> [op](auto term) {
        return unit(Xi_Expr{Xi_Unop{.expr{term}, .op{op}}});
    };
};

auto Xi_mathexpr(std::string_view input) -> Parsed_t<Xi_Expr> {
    return (Xi_term >> [](Xi_Expr lhs) {
        return (some(Xi_addop, binop_fold) >>
                [lhs](const Xi_Expr &rhs) {
                    auto [op, rhs_expr] = Unwarp_Unop(rhs);
                    return unit(
                        Xi_Expr{Xi_Binop{.lhs{lhs}, .rhs{rhs_expr}, .op{op}}});
                }) ||
               unit(lhs);
    })(input);
}

// <boolexpr> ::= <boolterm> | <boolterm> "||" <boolterm>
// <boolterm> ::= <boolfactor> | <boolfactor> "&&" <boolfactor>
// <boolfactor> ::= <boolvalue> | "!" <boolvalue>
// <boolvalue> ::= <boolean> | <mathexpr> <cmpop> <mathexpr> | "(" <boolexpr>
// ")"
auto Xi_boolexpr(std::string_view input) -> Parsed_t<Xi_Expr>;

const auto Xi_mathbool = Xi_mathexpr >> [](auto lhs) {
    return (Xi_eq || Xi_lt || Xi_le || Xi_gt || Xi_ge || Xi_ne) >>
           [lhs](auto op) {
               return Xi_mathexpr >> [lhs, op](auto rhs) {
                   return unit(
                       Xi_Expr{Xi_Binop{.lhs{lhs}, .rhs{rhs}, .op{op}}});
               };
           };
};

const auto Xi_boolvalue = Xi_boolean || Xi_mathbool || Xi_iden ||
                          (token(s_lparen) > Xi_boolexpr >> [](auto expr) {
                              return token(s_rparen) >> [expr](auto) {
                                  return unit(expr);
                              };
                          });

const auto Xi_boolfactor =
    Xi_boolvalue || (Xi_not > Xi_boolvalue >> [](auto expr) {
        return unit(Xi_Expr{Xi_Unop{.expr{expr}, .op{Xi_Op::Not}}});
    });

const auto Xi_boolterm =
    (Xi_boolfactor >>
     [](auto lhs) {
         return Xi_and >> [lhs](auto) {
             return Xi_boolfactor >> [lhs](auto rhs) {
                 return unit(
                     Xi_Expr{Xi_Binop{.lhs{lhs}, .rhs{rhs}, .op{Xi_Op::And}}});
             };
         };
     }) ||
    Xi_boolfactor;

// parse bool expression
auto Xi_boolexpr(std::string_view input) -> Parsed_t<Xi_Expr> {
    return ((Xi_boolterm >>
             [](auto lhs) {
                 return (Xi_and || Xi_or) >> [lhs](auto op) {
                     return Xi_boolterm >> [lhs, op](auto rhs) {
                         return unit(
                             Xi_Expr{Xi_Binop{.lhs{lhs}, .rhs{rhs}, .op{op}}});
                     };
                 };
             }) ||
            Xi_boolterm)(input);
};

} // namespace xi
