#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/expr.h>

namespace xi
{

const auto Xi_assign = s_iden >> [](std::string name)
{
    return token(symbol('=')) > Xi_expr >> [=](Xi_Expr expr)
    {
        return unit(Xi_Iden{
            .name = name,
            .expr = expr,
        });
    };
};

const auto Xi_let = token(str("let")) > some(Xi_assign) >>
                    [](std::vector<Xi_Iden> idens)
{
    return token(str("in")) > unit(idens);
};

// func ::= <iden> <type>* = <expr>
const auto Xi_func = s_iden >> [](std::string name)
{
    return many(s_iden) >> [name](std::vector<std::string> params)
    {
        return token(s_equals) > maybe(Xi_let) >>
               [name, params](std::optional<std::vector<Xi_Iden>> idens)
        {
            return Xi_expr >> [name, params, idens](Xi_Expr expr)
            {
                return unit(Xi_Stmt{Xi_Func{
                    .name      = name,
                    .params    = params,
                    .expr      = std::move(expr),
                    .let_idens = idens.value_or(std::vector<Xi_Iden>{}),
                }});
            };
        };
    };
};
} // namespace xi
