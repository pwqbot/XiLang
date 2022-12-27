#pragma once

#include "compiler/ast/expr/expr.h"
#include "compiler/ast/error.h"

namespace xi
{

struct Xi_Return
{
    Xi_Expr       expr;
    type::Xi_Type type                                 = type::unknown{};
    auto          operator<=>(const Xi_Return &) const = default;
};

auto TypeAssign(Xi_Return &stmt) -> TypeAssignResult;

} // namespace xi
