#pragma once

#include "compiler/ast/error.h"
#include "compiler/ast/expr/expr.h"
#include "compiler/ast/expr/iden.h"

namespace xi
{

struct Xi_Lam
{
    std::vector<Xi_Iden> args;
    Xi_Expr              body;
    type::Xi_Type        type = type::unknown{};
};

inline auto operator<=>(const Xi_Lam &lhs, const Xi_Lam &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.args <=> rhs.args; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.body <=> rhs.body;
}

auto TypeAssign(Xi_Lam & /*unused*/, LocalVariableRecord /*unused*/)
    -> TypeAssignResult;

} // namespace xi
