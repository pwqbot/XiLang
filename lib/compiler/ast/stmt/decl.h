#pragma once

#include "compiler/ast/iden.h"
#include "compiler/ast/type.h"
#include "compiler/ast/type_assign.h"

#include <string>
#include <vector>

namespace xi
{

struct Xi_Decl
{
    std::string              name;
    std::string              return_type;
    std::vector<std::string> params_type;
    bool                     is_vararg = false;
    type::Xi_Type            type      = type::unknown{};
    auto                     operator<=>(const Xi_Decl &) const = default;
};

auto TypeAssign(Xi_Decl &decl) -> TypeAssignResult
{
    if (GetSymbolTable().contains({decl.name, SymbolType::Function}))
    {
        return tl::make_unexpected(TypeAssignError{
            TypeAssignError::DuplicateDeclaration,
            fmt::format("Decl {}", decl.name),
            &decl,
        });
    }

    return findTypeInSymbolTable(decl.return_type, SymbolType::Type, &decl) >>=
           [&decl](auto return_type)
    {
        return flatmap(
                   decl.params_type,
                   [decl](auto x)
                   {
                       return findTypeInSymbolTable(x, SymbolType::Type, &decl);
                   }
               ) >>= [return_type, &decl](std::vector<type::Xi_Type> param_types
                     ) -> TypeAssignResult
        {
            auto func_type = type::function{
                .return_type = return_type,
                .param_types = param_types,
            };

            auto vararg_iter =
                ranges::find(param_types, type::Xi_Type{type::vararg{}});
            if (vararg_iter != param_types.end())
            {
                if (*vararg_iter != param_types.back())
                {
                    return tl::make_unexpected(TypeAssignError{
                        TypeAssignError::VarargNotLast,
                        fmt::format("vararg must be the last param"),
                        &decl,
                    });
                }
                func_type.param_types.pop_back();
                func_type.is_vararg = true;
            }

            GetSymbolTable().insert(
                {{decl.name, SymbolType::Function}, func_type}
            );
            return decl.type = func_type;
        };
    };
}

} // namespace xi
