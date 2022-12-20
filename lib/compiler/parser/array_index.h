#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/num.h>
#include <compiler/parser/expr.h>

namespace xi
{

// <iden> "." <iden>
inline const auto Xi_arrayIndex = s_iden >> [](std::string var_name)
{
    return token(symbol('[')) > Xi_expr >> [var_name](Xi_Expr index)
    {
        return token(symbol(']')) > unit(Xi_Expr{
                                        Xi_ArrayIndex{
                                            .array_var_name = var_name,
                                            .index          = index
                                        },
                                    });
    };
};

} // namespace xi
