
#include "compiler/ast/stmt/func.h"

#include "compiler/ast/all.h"
#include "compiler/ast/error.h"

namespace xi
{

auto TypeAssign(Xi_Func &func_def) -> TypeAssignResult
{
    if (GetFunctionDefinitionTable().contains(func_def.name))
    {
        return tl::make_unexpected(TypeAssignError{
            TypeAssignError::DuplicateDefinition,
            fmt::format("Func {}", func_def.name),
        });
    }

    return findTypeInSymbolTable(func_def.name, SymbolType::Function) >>=
           [&func_def](auto Xi_Type_decl_type)
    {
        return std::visit(
            [&func_def]<typename T>(T decl_type_wrapper) -> TypeAssignResult
            {
                if constexpr (std::same_as<
                                  std::decay_t<T>,
                                  recursive_wrapper<type::function>>)
                {
                    auto decl_type = decl_type_wrapper.get();
                    if (decl_type.param_types.size() != func_def.params.size())
                    {
                        return tl::make_unexpected(TypeAssignError{
                            TypeAssignError::ParameterCountMismatch,
                            fmt::format(
                                "expect {} params, find {}",
                                decl_type.param_types.size(),
                                func_def.params.size()
                            ),
                        });
                    }

                    LocalVariableRecord record;
                    for (const auto &[decl_param, func_param] :
                         ranges::views::zip(
                             decl_type.param_types, func_def.params
                         ))
                    {
                        if (record.contains(func_param))
                        {
                            return tl::make_unexpected(TypeAssignError{
                                TypeAssignError::DuplicateDeclaration,
                                fmt::format(
                                    "Decl function parameter {}", func_param
                                ),
                            });
                        }
                        record.insert({func_param, decl_param});
                    }

                    return flatmap_(
                               func_def.let_idens,
                               [record](auto &x)
                               {
                                   return TypeAssign(x.expr, record);
                               }
                           ) >>=
                           [&func_def, record, decl_type](
                               std::vector<type::Xi_Type> let_expr_types
                           ) mutable -> TypeAssignResult
                    {
                        for (auto &&[let_var, let_expr_type] :
                             ranges::views::zip(
                                 func_def.let_idens, let_expr_types
                             ))
                        {
                            if (record.contains(let_var.name))
                            {
                                return tl::make_unexpected(TypeAssignError{
                                    TypeAssignError::DuplicateDeclaration,
                                    fmt::format(
                                        "Decl let variable {}", let_var.name
                                    ),
                                });
                            }

                            let_var.type = let_expr_type;
                            record.insert({let_var.name, let_expr_type});
                        }

                        return TypeAssign(func_def.expr, record) >>=
                               [decl_type, &func_def](auto func_def_expr_type
                               ) -> TypeAssignResult
                        {
                            if (func_def_expr_type != decl_type.return_type)
                            {
                                return tl::make_unexpected(TypeAssignError{
                                    TypeAssignError::TypeMismatch,
                                    fmt::format(
                                        "expect return {}, find {}",
                                        decl_type.return_type,
                                        func_def_expr_type
                                    ),
                                });
                            }
                            GetFunctionDefinitionTable().insert(
                                {func_def.name, func_def.type}
                            );
                            return func_def.type = decl_type;
                        };
                    };
                }
                else
                {
                    return tl::make_unexpected(TypeAssignError{
                        TypeAssignError::TypeMismatch,
                        fmt::format(
                            "expect function, find {}", decl_type_wrapper
                        ),
                    });
                }
            },
            Xi_Type_decl_type
        );
    };
}

} // namespace xi
