#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/parser_combinator.h>

namespace xi
{

inline const Parser auto
    type_s = token(
                 some(s_alphanum || s_underscore || s_lbracket || s_rbracket) ||
                 str("...")
             ) >>
             [](std::string name) -> Parser auto
{
    return unit(name);
};

inline const Parser auto decl_term_s = type_s >> [](auto t)
{
    return token(str("->")) > unit(t);
};

// decl ::= "fn" <iden> "::" <arg_type>* <type>
// <arg_type> = <type> "->"
inline const auto Xi_decl = token(str("fn")) > Xi_iden >>
                            [](const Xi_Iden &name)
{
    return token(str("::")) > many(decl_term_s) >>
           [name](std::vector<std::string> param_types)
    {
        return type_s >> [name, param_types](std::string return_type)
        {
            return unit(Xi_Stmt{Xi_Decl{
                .name        = name,
                .return_type = return_type,
                .params_type = param_types,
            }});
        };
    };
};
} // namespace xi
