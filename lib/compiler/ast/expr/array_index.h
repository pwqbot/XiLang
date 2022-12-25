#pragma once

#include "compiler/ast/expr/expr.h"
#include "compiler/ast/error.h"

#include <compare>
#include <string>

namespace xi
{

struct Xi_ArrayIndex
{
    std::string   array_var_name;
    Xi_Expr       index;
    type::Xi_Type type = type::unknown{};
};

inline auto operator<=>(const Xi_ArrayIndex &lhs, const Xi_ArrayIndex &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.array_var_name <=> rhs.array_var_name; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.index <=> rhs.index;
}

auto TypeAssign(Xi_ArrayIndex &index, LocalVariableRecord record)
    -> TypeAssignResult;

} // namespace xi
