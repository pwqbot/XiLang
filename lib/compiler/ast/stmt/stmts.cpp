
#include "compiler/ast/stmt/stmts.h"

#include "compiler/ast/all.h"

namespace xi
{

auto TypeAssign(Xi_Stmts &stmt)
    -> TypeAssignResult
{
    auto types = flatmap_(
        stmt.stmts,
        [](auto &x)
        {
            return TypeAssign(x);
        }
    );
    if (types.has_value())
    {
        return types.value()[0];
    }
    return {};
}

} // namespace xi
