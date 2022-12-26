#include "compiler/ast/stmt/stmt.h"

#include "compiler/ast/all.h"
#include "compiler/ast/error.h"
#include "compiler/ast/type_assign.h"

namespace xi
{

auto TypeAssign(Xi_Stmt &stmt) -> TypeAssignResult
{
    return std::visit(
        [](auto& x) -> TypeAssignResult
        {
            return TypeAssign(x);
        },
        stmt
    );
}

} // namespace xi
