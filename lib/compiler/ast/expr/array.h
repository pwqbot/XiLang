#pragma once

#include "compiler/ast/error.h"
#include "compiler/ast/expr/expr.h"

#include <vector>

namespace xi
{

struct Xi_Array
{
    size_t               size = 0;
    std::vector<Xi_Expr> elements;
    type::Xi_Type        type = type::unknown{};
};

inline auto operator<=>(const Xi_Array &lhs, const Xi_Array &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.type <=> rhs.type; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.elements <=> rhs.elements;
}

auto TypeAssign(Xi_Array &arr, LocalVariableRecord record) -> TypeAssignResult;

} // namespace xi
