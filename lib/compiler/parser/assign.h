#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/expr.h>
#include <compiler/parser/parser_combinator.h>

namespace xi
{

inline const auto Xi_basic_assign = s_iden >> [](std::string name)
{
    return token(symbol('=')) > Xi_expr >> [name](Xi_Expr expr)
    {
        return unit(Xi_Expr(Xi_Assign{
            .name = name,
            .expr = expr,
        }));
    };
};

inline const auto Xi_compound_assign = s_iden >> [](std::string name)
{
    return (op("+") || op("-") || op("*") || op("/") || op("%") || op("&") ||
            op("|") || op("^")) >>
           [name](Xi_Op op)
    {
        return token(symbol('=')) > Xi_expr >> [op, name](Xi_Expr expr)
        {
            return unit(Xi_Expr(Xi_Assign({
                .name = name,
                .expr =
                    Xi_Binop{
                        .lhs = Xi_Iden{.name = name, .expr = std::monostate{}},
                        .rhs = expr,
                        .op  = op,
                    },
            })));
        };
    };
};

inline const auto Xi_assign = Xi_basic_assign || Xi_compound_assign;

} // namespace xi
