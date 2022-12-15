#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/expr.h>

namespace xi
{

// func ::= <iden> <type>* = <expr>
const auto Xi_func = Xi_iden >> [](Xi_Iden name)
{
    return many(Xi_iden) >> [name](std::vector<Xi_Iden> params)
    {
        return token(s_equals) > Xi_expr >> [name, params](Xi_Expr expr)
        {
            return unit(Xi_Stmt{Xi_Func{
                .name        = name,
                .params = params,
                .expr        = std::move(expr),
            }});
        };
    };
};
} // namespace xi
