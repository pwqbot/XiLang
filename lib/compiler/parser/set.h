
#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/expr.h>
#include <compiler/parser/parser_combinator.h>
#include <magic_enum.hpp>

namespace xi
{

inline const Parser auto Xi_set_type = token(some(s_alphanum || s_underscore)
                                       ) >>
                                       [](const std::string &name)
{
    return unit(name);
};

inline const auto Xi_user_type_term = token(symbol('(')) > Xi_set_type >>
                                      [](std::string name)
{
    return Xi_set_type >> [name](std::string type)
    {
        return token(symbol(')')) > unit(std::make_pair(name, type));
    };
};

// decl ::= "set <iden> = { (<iden> : <Type>)+  }"
inline const auto Xi_set = token(str("set")) > Xi_iden >> [](Xi_Iden name)
{
    return token(symbol('=')) > token(symbol('(')) > some(Xi_user_type_term) >>
           [name](std::vector<std::pair<std::string, std::string>> members)
    {
        return token(symbol(')')) > unit(Xi_Stmt{Xi_Set{
                                        .name    = name,
                                        .members = members,
                                    }});
    };
};
} // namespace xi
