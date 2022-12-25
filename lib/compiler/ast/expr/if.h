#pragma once

#include "compiler/ast/error.h"
#include "compiler/ast/expr/expr.h"
#include "compiler/ast/type.h"

namespace xi
{

struct Xi_If
{
    Xi_Expr       cond;
    Xi_Expr       then;
    Xi_Expr       els;
    type::Xi_Type type = type::unknown{};
};

inline auto operator<=>(const Xi_If &lhs, const Xi_If &rhs)
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

auto TypeAssign(Xi_If &if_expr, LocalVariableRecord record) -> TypeAssignResult;

} // namespace xi
