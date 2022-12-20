#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/expr.h>
#include <compiler/parser/parser_combinator.h>

namespace xi
{

inline const auto Xi_exprBreakNewline = s_whitespace >>
                                        [](std::string spaces) -> Parser auto
{
    return [spaces](std::string_view input) -> Parsed_t<Xi_Expr>
    {
        if (spaces.contains('\n'))
        {
            return std::nullopt;
        }
        return Xi_expr(input);
    };
};

inline const auto Xi_call = Xi_iden >> [](const Xi_Iden &iden)
{
    return token(symbol('@')) >
           many(
               Xi_exprBreakNewline,
               [](std::vector<Xi_Expr> lhs, const Xi_Expr &rhs)
               {
                   lhs.push_back(rhs);
                   return lhs;
               },
               std::vector<Xi_Expr>{}
           ) >>
           [iden](const std::vector<Xi_Expr> &args)
    {
        return unit(Xi_Expr{Xi_Call{iden, args}});
    };
};
} // namespace xi
