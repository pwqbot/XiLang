
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

    return unit(Xi_Type::_set);
};

inline const Parser auto Xi_decl_term = Xi_type >> [](auto t)
{
    return token(str("->")) > unit(t);
};

inline const auto Xi_user_type_term = token(symbol('(')) > Xi_iden >>
                                      [](Xi_Iden name)
{
    return Xi_type >> [name](Xi_Type type)
    {
        return token(symbol(')')) > unit(Xi_Iden{
                                        .name = name,
                                        .type = type,
                                    });
    };
};

// decl ::= "set <iden> = { (<iden> : <Type>)+  }"
inline const auto Xi_set = token(str("set")) > Xi_iden >> [](Xi_Iden name)
{
    return token(symbol('=')) > token(symbol('(')) > some(Xi_user_type_term) >>
           [name](std::vector<Xi_Iden> members)
    {
        return token(symbol(')')) > unit(Xi_Set{
                                        .name    = name,
                                        .members = members,
                                    });
    };
};
} // namespace xi
