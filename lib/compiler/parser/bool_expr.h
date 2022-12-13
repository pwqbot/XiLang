#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/math_expr.h>

namespace xi
{

// <boolexpr> ::= <boolterm> | <boolterm> "||" <boolterm>
// <boolterm> ::= <boolfactor> | <boolfactor> "&&" <boolfactor>
// <boolfactor> ::= <boolvalue> | "!" <boolvalue>
// <boolvalue> ::= <boolean> | <mathexpr> <cmpop> <mathexpr> | "(" <boolexpr>
// ")"
auto Xi_boolexpr(std::string_view input) -> Parsed_t<Xi_Expr>;

inline const Parser auto Xi_true = token(str("true")) >> [](auto)
{
    return unit(Xi_Expr{Xi_Boolean{true}});
};

inline const Parser auto Xi_false = token(str("false")) >> [](auto)
{
    return unit(Xi_Expr{Xi_Boolean{false}});
};

inline const Parser auto Xi_boolean = Xi_true || Xi_false;

inline const auto Xi_mathbool = Xi_mathexpr >> [](auto lhs)
{
    return (Xi_eq || Xi_lt || Xi_le || Xi_gt || Xi_ge || Xi_ne) >>
           [lhs](auto op)
    {
        return Xi_mathexpr >> [lhs, op](auto rhs)
        {
            return unit(Xi_Expr{Xi_Binop{
                .lhs = lhs,
                .rhs = rhs,
                .op  = op,
            }});
        };
    };
};

inline const auto Xi_boolvalue = Xi_boolean || Xi_mathbool || Xi_iden ||
                                 (token(s_lparen) > Xi_boolexpr >>
                                  [](auto expr)
                                  {
                                      return token(s_rparen) >> [expr](auto)
                                      {
                                          return unit(expr);
                                      };
                                  });

inline const auto Xi_boolfactor =
    Xi_boolvalue || (Xi_not > Xi_boolvalue >>
                     [](auto expr)
                     {
                         return unit(Xi_Expr{Xi_Unop{
                             .expr = expr,
                             .op   = Xi_Op::Not,
                         }});
                     });

inline const auto Xi_boolterm = (Xi_boolfactor >>
                                 [](auto lhs)
                                 {
                                     return Xi_and >> [lhs](auto)
                                     {
                                         return Xi_boolfactor >> [lhs](auto rhs)
                                         {
                                             return unit(Xi_Expr{Xi_Binop{
                                                 .lhs = lhs,
                                                 .rhs = rhs,
                                                 .op  = Xi_Op::And,
                                             }});
                                         };
                                     };
                                 }) ||
                                Xi_boolfactor;

// parse bool expression
inline auto Xi_boolexpr(std::string_view input) -> Parsed_t<Xi_Expr>
{
    return (
        (Xi_boolterm >>
         [](auto lhs)
         {
             return (Xi_and || Xi_or) >> [lhs](auto op)
             {
                 return Xi_boolterm >> [lhs, op](auto rhs)
                 {
                     return unit(Xi_Expr{
                         Xi_Binop{
                             .lhs = lhs,
                             .rhs = rhs,
                             .op  = op,
                         },
                     });
                 };
             };
         }) ||
        Xi_boolterm
    )(input);
};

} // namespace xi
