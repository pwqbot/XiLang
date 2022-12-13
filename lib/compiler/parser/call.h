#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/expr.h>
#include <compiler/parser/parser_combinator.h>

namespace xi
{

inline const auto Xi_call = Xi_iden >> [](const Xi_Iden &iden)
{
    return token(s_lparen) >
           many(
               Xi_expr,
               [](std::vector<Xi_Expr> lhs, const Xi_Expr &rhs)
               {
                   lhs.push_back(rhs);
                   return lhs;
               },
               std::vector<Xi_Expr>{}
           ) >>
           [iden](const std::vector<Xi_Expr> &args)
    {
        return token(s_rparen) >> [iden, args](auto)
        {
            return unit(Xi_Expr{Xi_Call{iden, args}});
        };
    };
};
}
