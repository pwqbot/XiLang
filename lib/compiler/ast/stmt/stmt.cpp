#include "compiler/ast/stmt/stmt.h"

#include "compiler/ast/all.h"
#include "compiler/ast/error.h"
#include "compiler/ast/type_assign.h"

namespace xi
{

auto TypeAssign(Xi_Stmt &stmt, LocalVariableRecord &record) -> TypeAssignResult
{
    return std::visit(
        [&record](auto &x) -> TypeAssignResult
        {
            return TypeAssign(x, record);
        },
        stmt
    );
}

} // namespace xi
