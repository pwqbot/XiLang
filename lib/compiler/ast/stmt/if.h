
#include "compiler/ast/expr/expr.h"
#include "compiler/ast/stmt/stmt.h"

namespace xi
{

struct Xi_If_stmt
{
    Xi_Expr              cond;
    std::vector<Xi_Stmt> then;
    std::vector<Xi_Stmt> els;
};

inline auto operator<=>(const Xi_If_stmt &lhs, const Xi_If_stmt &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.cond <=> rhs.cond; cmp != nullptr)
    {
        return cmp;
    }
    if (auto cmp = lhs.then <=> rhs.then; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.els <=> rhs.els;
}

auto TypeAssign(Xi_If_stmt &stmt, LocalVariableRecord = {}) -> TypeAssignResult;

} // namespace xi
