#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/expr.h>
#include <compiler/parser/parser_combinator.h>
#include <magic_enum.hpp>

namespace xi
{

// array ::= "fn" <iden> "::" <arg_type>* <type>
// <empty_decl> = "[" <expr>* "]"
// <comprehension> = "[" <l..r> "]"

inline const auto
    Xi_array = token(s_lbracket) >
               ((Xi_expr >>
                 [](auto first_expr)
                 {
                     return many(token(symbol(',')) > Xi_expr) >>
                            [first_expr](std::vector<Xi_Expr> exprs)
                     {
                         exprs.insert(exprs.begin(), first_expr);
                         return token(s_rbracket) >
                                unit(Xi_Expr{
                                    Xi_Array{
                                        .size     = exprs.size(),
                                        .elements = std::move(exprs),
                                    },
                                });
                     };
                 }) ||
                (token(s_rbracket) > unit(Xi_Expr{
                                         Xi_Array{
                                             .size     = 0,
                                             .elements = {},
                                         },
                                     })));

} // namespace xi
// namespace xi
