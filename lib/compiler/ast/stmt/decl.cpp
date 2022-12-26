
#include "compiler/ast/stmt/decl.h"

#include "compiler/ast/error.h"

namespace xi
{

auto TypeAssign(Xi_Decl &decl) -> TypeAssignResult
{
    if (GetSymbolTable().contains({decl.name, SymbolType::Function}))
    {
        return tl::make_unexpected(TypeAssignError{
            TypeAssignError::DuplicateDeclaration,
            fmt::format("Decl {}", decl.name),
        });
    }

    return findTypeInSymbolTable(decl.return_type, SymbolType::Type) >>=
           [&decl](auto return_type)
    {
        return flatmap(
                   decl.params_type,
                   [decl](auto x)
                   {
                       return findTypeInSymbolTable(x, SymbolType::Type);
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
