
#include "compiler/ast/stmt/stmts.h"

#include "compiler/ast/all.h"

namespace xi
{

auto TypeAssign(Xi_Stmts &stmt, LocalVariableRecord &record) -> TypeAssignResult
{
    auto types = flatmap_(
        stmt.stmts,
        [&record](auto &x)
        {
            return TypeAssign(x, record);
        }
    );
    if (types.has_value())
    {
        return types.value()[0];
    }
    return {};
}

} // namespace xi
