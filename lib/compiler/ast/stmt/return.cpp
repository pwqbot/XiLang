#include "compiler/ast/all.h"
#include "compiler/ast/error.h"

namespace xi
{

auto TypeAssign(Xi_Return &stmt) -> TypeAssignResult
{
    // TODO(ding.wang): fix this
    return TypeAssign(stmt.expr);
}

} // namespace xi
