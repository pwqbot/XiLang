#include "compiler/ast/expr/assign.h"

#include "compiler/ast/all.h"
#include "compiler/ast/ast_format.h"
#include "compiler/ast/error.h"

namespace xi
{

auto TypeAssign(Xi_Assign &assign_expr, LocalVariableRecord record)
    -> TypeAssignResult
{
    auto tmpIden = Xi_Expr{Xi_Iden{
        .name = assign_expr.name,
        .expr = std::monostate{},
    }};
    return TypeAssign(tmpIden, record) >>=
           [&assign_expr, record](type::Xi_Type iden_type)
    {
        return TypeAssign(assign_expr.expr, record) >>=
               [iden_type,
                &assign_expr](type::Xi_Type expr_type) -> TypeAssignResult
        {
            if (iden_type != expr_type)
            {
                return tl::make_unexpected(TypeAssignError{
                    TypeAssignError::TypeMismatch,
                    fmt::format("cannot assign {} to {}", expr_type, iden_type),
                });
            }
            return assign_expr.type = iden_type;
        };
    };
}
} // namespace xi
