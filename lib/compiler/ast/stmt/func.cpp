
#include "compiler/ast/stmt/func.h"

#include "compiler/ast/all.h"
#include "compiler/ast/error.h"

namespace xi
{

template <typename T>
auto hasDuplicate(std::vector<T> v)
{
    std::sort(v.begin(), v.end());
    return std::adjacent_find(v.begin(), v.end()) != v.end();
}

auto typeAssignExprFunc(
    Xi_Func &func_def, LocalVariableRecord record, type::function decl_type
) -> TypeAssignResult
{
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
        if (hasDuplicate(func_def.let_idens))
        {
            return tl::make_unexpected(TypeAssignError{
                TypeAssignError::DuplicateDeclaration,
                fmt::format("Decl function parameter {}", func_def.name),
            });
        }
        for (auto &&[let_var, let_expr_type] :
             ranges::views::zip(func_def.let_idens, let_expr_types))
        {
            if (record.contains(let_var.name))
            {
                return tl::make_unexpected(TypeAssignError{
                    TypeAssignError::DuplicateDeclaration,
                    fmt::format("{} function let variable", let_var.name),
                });
            }

            let_var.type = let_expr_type;
            record.insert_or_assign(let_var.name, let_expr_type);
        }

        return TypeAssign(func_def.expr, record) >>=
               [decl_type,
                &func_def](auto func_def_expr_type) -> TypeAssignResult
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
            GetFunctionDefinitionTable().insert({func_def.name, decl_type});
            return func_def.type = decl_type;
        };
    };
}

auto typeAssignDeclFunc(
    Xi_Func &func_def, LocalVariableRecord record, type::function decl_type
) -> TypeAssignResult
{
    GetCurrentFuncType() = decl_type;
    return flatmap_(
               func_def.stmts,
               [&record](auto &x)
               {
                   return TypeAssign(x, record);
               }
           ) >>=
           [&func_def,
            record,
            decl_type](std::vector<type::Xi_Type>) mutable -> TypeAssignResult
    {
        GetFunctionDefinitionTable().insert({func_def.name, decl_type});
        return func_def.type = decl_type;
    };
}

auto TypeAssign(Xi_Func &func_def, LocalVariableRecord record)
    -> TypeAssignResult
{
    if (GetFunctionDefinitionTable().contains(func_def.name))
    {
        return tl::make_unexpected(TypeAssignError{
            TypeAssignError::DuplicateDefinition,
            fmt::format("Func {}", func_def.name),
        });
    }

    return findTypeInSymbolTable(func_def.name, SymbolType::Function) >>=
           [&func_def, &record](auto Xi_Type_decl_type)
    {
        return std::visit(
            [&func_def,
             &record]<typename T>(T decl_type_wrapper) -> TypeAssignResult
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

                    if (hasDuplicate(func_def.params))
                    {
                        return tl::make_unexpected(TypeAssignError{
                            TypeAssignError::DuplicateDeclaration,
                            fmt::format(
                                "{} function parameter ", func_def.name
                            ),
                        });
                    }

                    for (const auto &[decl_param, func_param] :
                         ranges::views::zip(
                             decl_type.param_types, func_def.params
                         ))
                    {
                        record.insert_or_assign(func_param, decl_param);
                    }

                    if (func_def.expr == std::monostate{})
                    {
                        return typeAssignDeclFunc(func_def, record, decl_type);
                    }
                    return typeAssignExprFunc(func_def, record, decl_type);
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
