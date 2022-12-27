#pragma once

#include "compiler/ast/error.h"
#include "compiler/ast/expr/expr.h"

namespace xi
{

struct Xi_Return
{
    Xi_Expr       expr;
    type::Xi_Type type                                 = type::unknown{};
    auto          operator<=>(const Xi_Return &) const = default;
};

auto TypeAssign(Xi_Return &stmt, LocalVariableRecord = {}) -> TypeAssignResult;

} // namespace xi
