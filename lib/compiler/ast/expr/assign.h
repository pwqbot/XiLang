#pragma once

#include "compiler/ast/expr/expr.h"
#include "compiler/ast/type.h"

namespace xi
{

struct Xi_Assign
{
    std::string   name;
    Xi_Expr       expr;
    type::Xi_Type type = type::unknown{};
};

inline auto operator<=>(const Xi_Assign &lhs, const Xi_Assign &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.name <=> rhs.name; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.expr <=> rhs.expr;
}

auto TypeAssign(Xi_Assign &, LocalVariableRecord) -> TypeAssignResult;

} // namespace xi
