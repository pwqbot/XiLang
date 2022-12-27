#pragma once

#include "compiler/ast/all.h"
#include "compiler/parser/basic_parsers.h"

#include <compiler/ast/ast.h>
#include <compiler/parser/expr.h>
#include <string_view>
#include <vector>

namespace xi
{
auto Xi_block_stmt_(std::string_view input) -> Parsed_t<Xi_Stmt>;
auto Xi_top_stmt_(std::string_view input) -> Parsed_t<Xi_Stmt>;

const auto Xi_block_stmt = Xi_block_stmt_ >> [](auto stmt)
{
    return unit(std::move(stmt));
};

const auto Xi_top_stmt = Xi_top_stmt_ >> [](auto stmt)
{
    return unit(std::move(stmt));
};

const auto Xi_exprStmt = Xi_expr >> [](auto expr)
{
    return token(symbol(';')) > unit(Xi_Stmt{expr});
};

const auto Xi_return = token(str("return")) > Xi_expr >> [](Xi_Expr expr)
{
    return token(symbol(';')) > unit(Xi_Stmt{
                                    Xi_Return{
                                        expr,
                                    },
                                });
};

const auto Xi_while = token(str("while")) > token(symbol('(')) > Xi_expr >>
                      [](Xi_Expr cond)
{
    return token(symbol(')')) > token(symbol('{')) > many(Xi_block_stmt) >>
           [=](std::vector<Xi_Stmt> body)
    {
        return token(symbol('}')) > unit(Xi_Stmt{
                                        Xi_While{
                                            .cond = cond,
                                            .body = body,
                                        },
                                    });
    };
};

// TODO(ding.wang): remove ()
const auto Xi_for = token(str("for")) > token(symbol('(')) > Xi_expr >>
                    [](Xi_Expr init)
{
    return token(symbol(';')) > Xi_expr >> [init](Xi_Expr cond)
    {
        return token(symbol(';')) > Xi_expr >> [init, cond](Xi_Expr step)
        {
            return token(symbol(')')) > token(symbol('{')) >
                   many(Xi_block_stmt) >>
                   [init, cond, step](std::vector<Xi_Stmt> body)
            {
                body.emplace_back(step);
                return token(symbol('}')) > unit(Xi_Stmt{
                                                Xi_Stmts{
                                                    {
                                                        init,
                                                        Xi_While{
                                                            .cond = cond,
                                                            .body = body,
                                                        },
                                                    },
                                                },
                                            });
            };
        };
    };
};

const auto Xi_program = many(Xi_top_stmt) >> [](auto progam)
{
    return unit(Xi_Program{progam});
};

} // namespace xi
