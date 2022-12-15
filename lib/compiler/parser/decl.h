#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/expr.h>
#include <compiler/parser/parser_combinator.h>
#include <magic_enum.hpp>

namespace xi
{

inline const Parser auto Xi_type = token(some(s_alphanum || s_underscore)) >>
                                   [](const std::string &name)
{
    auto t = magic_enum::enum_cast<Xi_Type>(name);
    if (t.has_value())
    {
        return unit(t.value());
    }

    return unit(Xi_Type::_user);
};

inline const Parser auto Xi_decl_term = Xi_type >> [](auto t)
{
    return token(str("->")) >> [t](auto)
    {
        return unit(t);
    };
};

// decl ::= "fn" <iden> "::" <arg_type>* <type>
// <arg_type> = <type> "->"
inline const auto Xi_decl = token(str("fn")) > Xi_iden >>
                            [](const Xi_Iden &name)
{
    return token(str("::")) > many(Xi_decl_term) >>
           [name](const std::vector<Xi_Type> &param_types)
    {
        return Xi_type >> [name, param_types](Xi_Type return_type)
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
