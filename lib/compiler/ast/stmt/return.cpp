#include "compiler/ast/all.h"
#include "compiler/ast/error.h"

namespace xi
{

auto TypeAssign(Xi_Return &stmt, LocalVariableRecord record) -> TypeAssignResult
{
    // TODO(ding.wang): fix this
    return TypeAssign(stmt.expr, record) >>=
           [&stmt](type::Xi_Type expr_type) -> TypeAssignResult
    {
        if (expr_type != GetCurrentFuncType().return_type)
        {
            return tl::make_unexpected(TypeAssignError{
                .err     = TypeAssignError::TypeMismatch,
                .message = fmt::format(
                    "return type mismatch, expect {}, found {}",
                    GetCurrentFuncType().return_type,
                    expr_type
                )});
        }
        return stmt.type = expr_type;
    };
}

} // namespace xi
