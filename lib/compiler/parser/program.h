#pragma once

#include "compiler/ast/all.h"
#include "compiler/parser/basic_parsers.h"
#include "compiler/parser/decl.h"

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

const auto Xi_top_stmt = Xi_top_stmt_ >> [](Xi_Stmt stmt)
{
    return unit(std::move(stmt));
};

const auto Xi_exprStmt = maybe(Xi_expr) >> [](std::optional<Xi_Expr> expr)
{
    return token(symbol(';')) > (expr
                                     ? unit(Xi_Stmt{expr.value()})
                                     : unit(Xi_Stmt{Xi_Comment{.text = ".."}}));
};

const auto Xi_compound_stmt = Xi_block_stmt >> [](Xi_Stmt stmt)
{
    return unit(Xi_Stmts{
        .stmts = {stmt},
    });
} || (token(symbol('{')) > many(Xi_block_stmt) >> [](std::vector<Xi_Stmt> stmts) {
        return token(symbol('}')) > unit(Xi_Stmts{
            .stmts = std::move(stmts),
        });
    });

const auto Xi_if_stmt = token(str("if")) > token(symbol('(')) > Xi_expr >>
                        [](Xi_Expr expr)
{
    return token(symbol(')')) > Xi_compound_stmt >> [expr](auto then_stmt)
    {
        return maybe(token(str("else")) > Xi_compound_stmt) >> [=](auto else_stmt)
        {
            return unit(Xi_Stmt{
                Xi_If_stmt{
                    .cond = expr,
                    .then = then_stmt.stmts,
                    .els  = else_stmt ? else_stmt.value().stmts
                                      : std::vector<Xi_Stmt>{}},
            });
        };
    };
};

const auto Xi_var = type_s >> [](std::string type)
{
    return s_iden >> [type](std::string name)
    {
        return maybe(token(str("=")) > Xi_expr) >> [name, type](auto expr)
        {
            return token(symbol(';')) >
                   unit(Xi_Stmt{
                       Xi_Var{
                           .name      = name,
                           .value     = expr.value_or(std::monostate{}),
                           .type_name = type,
                       },
                   });
        };
    };
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
