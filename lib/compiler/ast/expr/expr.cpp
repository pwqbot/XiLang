#include "compiler/ast/expr/expr.h"

#include "compiler/ast/all.h"
#include "compiler/ast/type_assign.h"

namespace xi
{

auto TypeAssign(Xi_Expr &expr, LocalVariableRecord record) -> TypeAssignResult
{
    return std::visit(
        [record](auto &expr_) mutable
        {
            return TypeAssign(expr_, record);
        },
        expr
    );
}
} // namespace xi
