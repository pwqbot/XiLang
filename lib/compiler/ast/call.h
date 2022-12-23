#pragma once

#include "compiler/ast/ast.h"
#include "compiler/ast/error.h"
#include "compiler/ast/type.h"

#include <string>
#include <vector>

namespace xi
{

struct Xi_Call
{
    std::string          name;
    std::vector<Xi_Expr> args;
    type::Xi_Type        type = type::unknown{};
};

inline auto operator<=>(const Xi_Call &lhs, const Xi_Call &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.args <=> rhs.args; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.name <=> rhs.name;
}

auto TypeAssign(Xi_Call call_expr, LocalVariableRecord record)
    -> TypeAssignResult;
} // namespace xi
