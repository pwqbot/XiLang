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

inline const auto Xi_decl = token(s_lbracket) > many(Xi_expr) >>
                            [](std::vector<Xi_Expr> exprs)
{
    return token(s_rbracket) > unit(Xi_Array{
                                   .size     = exprs.size(),
                                   .elements = std::move(exprs),
                               });
};

} // namespace xi
// namespace xi
