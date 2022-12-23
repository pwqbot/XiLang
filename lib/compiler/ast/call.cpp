#include "compiler/ast/call.h"

#include "compiler/ast/array_index.h"
#include "compiler/ast/ast_format.h"
#include "compiler/ast/error.h"
#include "compiler/functional/monad.h"

namespace xi
{

auto TypeAssign(Xi_Call call_expr, LocalVariableRecord record)
    -> TypeAssignResult
{
    return flatmap_(
               call_expr.args,
               [record](auto &x)
               {
                   return TypeAssign(x, record);
               }
           ) >>= [&call_expr, record](auto args_type)
    {
        return findTypeInSymbolTable(
                   call_expr.name, SymbolType::Function, call_expr
               ) >>= [args_type, &call_expr, record](auto func_type
                     ) -> TypeAssignResult
        {
            return std::visit(
                [&call_expr,
                 args_type]<typename T>(T &&func_type_) -> TypeAssignResult
                {
                    if constexpr (std::same_as<
                                      std::decay_t<T>,
                                      recursive_wrapper<type::function>>)
                    {
                        if (!func_type_.get().is_vararg &&
                            func_type_.get().param_types != args_type)
                        {
                            return tl::make_unexpected(TypeAssignError{
                                TypeAssignError::TypeMismatch,
                                fmt::format(
                                    "param type mismatch, expect {}, find "
                                    "{}",
                                    func_type_.get().param_types,
                                    args_type
                                ),
                                call_expr,
                            });
                        }
                        return call_expr.type = func_type_.get().return_type;
                    }
                    else
                    {
                        return tl::make_unexpected(TypeAssignError{
                            TypeAssignError::TypeMismatch,
                            fmt::format(
                                "expect function type, find {}", func_type_
                            ),
                            call_expr,
                        });
                    }
                },
                func_type
            );
        };
    };
}

} // namespace xi
