#pragma once
#include <compiler/ast.h>
#include <compiler/basic_parsers.h>
#include <compiler/parsers.h>
#include <utility>

namespace xi
{

// parse if expression: if cond then expr else expr
const auto Xi_if = token(s_if) >> [](auto)
{
    return Xi_boolexpr >> [](auto cond)
    {
        return token(s_then) >> [cond](auto)
        {
            return Xi_mathexpr >> [cond](auto then)
            {
                return token(s_else) >> [cond, then](auto)
                {
                    return Xi_mathexpr >> [cond, then](auto els)
                    {
                        return unit(Xi_Expr{
                            Xi_If{.cond{cond}, .then{then}, .els{els}}});
                    };
                };
            };
        };
    };
};

// parse lambda: "?" <args> "->" <expr>
// <args> ::= <iden> | <iden> <args>

auto Xi_args(std::string_view input) -> Parsed_t<Xi_Args>
{
    return (
        (Xi_iden >>
         [](Xi_Iden arg)
         {
             return Xi_args >> [arg](auto args)
             {
                 args.insert(args.begin(), arg);
                 return unit(Xi_Args{args});
             };
         }) ||
        Xi_iden >> [](Xi_Iden arg) { return unit(Xi_Args{std::move(arg)}); }
    )(input);
}

const auto Xi_lam = token(s_question) > Xi_args >> [](auto args)
{
    return token(s_arrow) >> [args](auto)
    {
        return Xi_expr >> [args](auto expr)
        {
            return unit(Xi_Expr{Xi_Lam{.args{args}, .body{expr}}});
        };
    };
};

auto Xi_expr(std::string_view input) -> Parsed_t<Xi_Expr>
{
    return (
        Xi_true || Xi_false || Xi_string || Xi_mathexpr || Xi_if ||
        Xi_boolexpr || Xi_lam
    )(input);
}

} // namespace xi
