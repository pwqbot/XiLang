
#include "compiler/ast/all.h"
#include "compiler/ast/error.h"

namespace xi
{

auto TypeAssign(Xi_If_stmt &if_stmt, LocalVariableRecord record)
    -> TypeAssignResult
{
    return TypeAssign(if_stmt.cond, record) >>=
           [&if_stmt, &record](type::Xi_Type cond_type) -> TypeAssignResult
    {
        if (cond_type != type::buer{})
        {
            return tl::make_unexpected(TypeAssignError{
                TypeAssignError::TypeMismatch,
                fmt::format("expect buer, find {}", cond_type),
            });
        }
        return flatmap_(
                   if_stmt.then,
                   [record](auto &x) mutable
                   {
                       return TypeAssign(x, record);
                   }
               ) >>=
               [cond_type, &if_stmt, record](auto then_type) -> TypeAssignResult
        {
            return flatmap_(
                       if_stmt.els,
                       [record](auto &x) mutable
                       {
                           return TypeAssign(x, record);
                       }
                   ) >>=
                   [then_type, &if_stmt](auto else_type) -> TypeAssignResult
            {
                else_type.insert(
                    else_type.begin(),
                    then_type.begin(),
                    then_type.end()
                );
                return if_stmt.type = type::types{else_type};
            };
        };
    };
}

} // namespace xi
