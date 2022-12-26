#pragma once
#include <compiler/ast/ast.h>
#include <compiler/parser/comment_parser.h>
#include <compiler/parser/decl.h>
#include <compiler/parser/expr.h>
#include <compiler/parser/func.h>
#include <compiler/parser/set.h>
#include <string_view>
#include <vector>

namespace xi
{
auto Xi_stmt(std::string_view input) -> Parsed_t<Xi_Stmt>;

auto Xi_exprStmt = Xi_expr >> [](auto expr)
{
    return token(symbol(';')) > unit(Xi_Stmt{expr});
};

auto Xi_while = token(str("while")) > token(symbol('(')) > Xi_expr >>
                [](Xi_Expr cond)
{
    return token(symbol(')')) > token(symbol('{')) > many(Xi_stmt) >>
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

const auto Xi_for = token(str("for")) > Xi_expr >> [](Xi_Expr init)
{
    return token(symbol(';')) > Xi_expr >> [init](Xi_Expr cond)
    {
        return token(symbol(';')) > Xi_expr >> [init, cond](Xi_Expr step)
        {
            return token(symbol('{')) > many(Xi_stmt) >>
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

auto Xi_stmt(std::string_view input) -> Parsed_t<Xi_Stmt>
{
    return (Xi_decl || Xi_set || Xi_func || Xi_exprStmt || Xi_comment)(input);
}

const auto Xi_program = many(Xi_stmt) >> [](auto progam)
{
    return unit(Xi_Program{progam});
};

} // namespace xi
