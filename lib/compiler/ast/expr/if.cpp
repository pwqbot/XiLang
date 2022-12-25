#include "compiler/ast/expr/if.h"

#include "compiler/ast/all.h"
#include "compiler/ast/error.h"
#include "compiler/ast/type.h"

namespace xi
{

auto TypeAssign(Xi_If &if_expr, LocalVariableRecord record) -> TypeAssignResult
{
    return TypeAssign(if_expr.cond, record) >>=
           [&if_expr, record](auto cond_type) -> TypeAssignResult
    {
        return TypeAssign(if_expr.then, record) >>=
               [cond_type, &if_expr, record](auto then_type) -> TypeAssignResult
        {
            return TypeAssign(if_expr.els, record) >>=
                   [cond_type, then_type, &if_expr](auto else_type
                   ) -> TypeAssignResult
            {
                if (cond_type != type::buer{})
                {
                    return tl::make_unexpected(TypeAssignError{
                        TypeAssignError::TypeMismatch,
                        fmt::format("expect buer, find {}", cond_type),
                    });
                }
                if (then_type != else_type)
                {
                    return tl::make_unexpected(TypeAssignError{
                        TypeAssignError::TypeMismatch,
                        fmt::format(
                            "expect same type, lhs: {}, rhs {}",
                            then_type,
                            else_type
                        ),
                    });
                }
                return if_expr.type = then_type;
            };
        };
    };
}
} // namespace xi
