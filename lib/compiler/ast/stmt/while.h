#pragma once

#include "compiler/ast/error.h"
#include "compiler/ast/expr/expr.h"
#include "compiler/ast/stmt/stmt.h"

namespace xi
{

struct Xi_While
{
    Xi_Expr              cond;
    std::vector<Xi_Stmt> body;
    type::Xi_Type        type = type::unknown{};
};

inline auto operator<=>(const Xi_While &lhs, const Xi_While &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.cond <=> rhs.cond; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.body <=> rhs.body;
}

auto TypeAssign(Xi_While &stmt) -> TypeAssignResult;

} // namespace xi
