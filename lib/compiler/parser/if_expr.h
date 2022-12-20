#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/expr.h>

namespace xi
{

// parse if expression: if cond then expr else expr
const auto Xi_if = token(s_if) > Xi_expr >> [](const Xi_Expr &cond)
{
    return token(s_then) >> [cond](auto)
    {
        return Xi_expr >> [cond](const Xi_Expr &then)
        {
            return token(s_else) >> [cond, then](auto)
            {
                return Xi_expr >> [cond, then](const Xi_Expr &els)
                {
                    return unit(Xi_Expr{
                        Xi_If{
                            .cond = cond,
                            .then = then,
                            .els  = els,
                        },
                    });
                };
            };
        };
    };
};

} // namespace xi
