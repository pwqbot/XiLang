#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/expr.h>
#include <compiler/parser/parser_combinator.h>
#include <utility>

namespace xi
{

// parse lambda: "?" <args> "->" <expr>
// <args> ::= <iden> | <iden> <args>
inline auto Xi_args(std::string_view input) -> Parsed_t<Xi_Args>
{
    return (
        (Xi_iden >>
         [](const Xi_Iden &arg)
         {
             return Xi_args >> [arg](Xi_Args args)
             {
                 args.insert(args.begin(), arg);
                 return unit(Xi_Args{args});
             };
         }) ||
        Xi_iden >> [](const Xi_Iden &arg) { return unit(Xi_Args{arg}); }
    )(input);
}

inline const auto Xi_lam = token(s_question) > Xi_args >> [](auto args)
{
    return token(s_arrow) >> [args](auto)
    {
        return Xi_expr >> [args](auto expr)
        {
            return unit(Xi_Expr{
                Xi_Lam{
                    .args = args,
                    .body = expr,
                },
            });
        };
    };
};

} // namespace xi
