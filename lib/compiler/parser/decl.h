#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/expr.h>
#include <compiler/parser/parser_combinator.h>
#include <magic_enum.hpp>

namespace xi
{

inline const Parser auto Xi_type =
    (token(str("...")) > unit(Xi_Type::_vararg)) ||
    (token(some(s_alphanum || s_underscore)) >>
     [](const std::string &name)
     {
         auto t = magic_enum::enum_cast<Xi_Type>(name);
         if (t.has_value())
         {
             return unit(t.value());
         }

         return unit(Xi_Type::_set);
     });

inline const Parser auto Xi_decl_term = Xi_type >> [](auto t)
{
    return token(str("->")) > unit(t);
};

// decl ::= "fn" <iden> "::" <arg_type>* <type>
// <arg_type> = <type> "->"
inline const auto Xi_decl = token(str("fn")) > Xi_iden >>
                            [](const Xi_Iden &name)
{
    return token(str("::")) > many(Xi_decl_term) >>
           [name](std::vector<Xi_Type> param_types)
    {
        return Xi_type >> [name, param_types](Xi_Type return_type)
        {
            auto [is_vararg, new_param_types] =
                [](auto old_param_types
                ) -> std::pair<bool, std::vector<Xi_Type>>
            {
                if (!old_param_types.empty() &&
                    old_param_types.back() == Xi_Type::_vararg)
                {
                    return {
                        true,
                        old_param_types | ranges::views::drop_last(1) |
                            ranges::to_vector};
                }
                return {false, old_param_types};
            }(param_types);
            return unit(Xi_Stmt{Xi_Decl{
                .name        = name,
                .return_type = return_type,
                .params_type = new_param_types,
                .is_vararg   = is_vararg,
            }});
        };
    };
};
} // namespace xi
