#pragma once

#include <compiler/basic_parsers.h>
#include <compiler/ast.h>

namespace xi {

// <mathexpr> ::= <term> | <term> "+-" <term>
// <term> ::= <number> | <number> "*/" <number>
// <number> ::= <integer> | <real> | "(" <mathexpr> ")"
auto Xi_mathexpr(std::string_view input) -> Parsed_t<Xi_Expr>;

auto Xi_number(std::string_view input) -> Parsed_t<Xi_Expr> {
    return (Xi_real || Xi_integer || (s_lparen > Xi_mathexpr >> [](auto expr) {
                return s_rparen >> [expr](auto) {
                    return unit(expr);
                };
            }))(input);
}

inline const Parser auto Xi_term =
    (Xi_number >>
     [](auto lhs) {
         return (Xi_mul || Xi_divide) >> [lhs](auto op) {
             return Xi_number >> [lhs, op](auto rhs) {
                 return unit(Xi_Expr{Xi_Binop{.lhs{lhs}, .rhs{rhs}, .op{op}}});
             };
         };
     }) ||
    Xi_number;

auto Xi_mathexpr(std::string_view input) -> Parsed_t<Xi_Expr> {
    return ((Xi_term >>
             [](auto lhs) {
                 return (Xi_add || Xi_minus) >> [lhs](auto op) {
                     return Xi_term >> [lhs, op](auto rhs) {
                         return unit(
                             Xi_Expr{Xi_Binop{.lhs{lhs}, .rhs{rhs}, .op{op}}});
                     };
                 };
             }) ||
            Xi_term)(input);
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

const auto Xi_boolvalue =
    Xi_boolean || Xi_mathbool || (s_lparen > Xi_boolexpr >> [](auto expr) {
        return s_rparen >> [expr](auto) {
            return unit(expr);
        };
    });

const auto Xi_factor =
    Xi_boolvalue || (Xi_not > Xi_boolvalue >> [](auto expr) {
        return unit(Xi_Expr{Xi_Unop{.expr{expr}, .op{Xi_Op::Not}}});
    });

const auto Xi_boolterm =
    (Xi_factor >>
     [](auto lhs) {
         return Xi_and >> [lhs](auto) {
             return Xi_factor >> [lhs](auto rhs) {
                 return unit(Xi_Expr{Xi_Binop{.lhs{lhs}, .rhs{rhs}, .op{Xi_Op::And}}});
             };
         };
     }) ||
    Xi_factor;

// parse bool expression
auto Xi_boolexpr(std::string_view input) -> Parsed_t<Xi_Expr> {
    return ((Xi_boolterm >>
     [](auto lhs) {
         return (Xi_and || Xi_or) >> [lhs](auto op) {
             return Xi_boolterm >> [lhs, op](auto rhs) {
                 return unit(Xi_Expr{Xi_Binop{.lhs{lhs}, .rhs{rhs}, .op{op}}});
             };
         };
     }) ||
    Xi_boolterm)(input);
};

// parse if expression: if cond then expr else expr
const auto Xi_if = s_if >> [](auto) {
    return Xi_boolexpr >> [](auto cond) {
        return s_then >> [cond](auto) {
            return Xi_mathexpr >> [cond](auto then) {
                return s_else >> [cond, then](auto) {
                    return Xi_mathexpr >> [cond, then](auto els) {
                        return unit(Xi_Expr{
                            Xi_If{.cond{cond}, .then{then}, .els{els}}});
                    };
                };
            };
        };
    };
};

const auto Xi_expr = Xi_true || Xi_false || Xi_string || Xi_mathexpr;

} // namespace xi
