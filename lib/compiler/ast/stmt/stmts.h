#pragma once

#include "compiler/ast/error.h"
#include "compiler/ast/stmt/stmt.h"

namespace xi
{

struct Xi_Stmts
{
    std::vector<Xi_Stmt> stmts;
};

inline auto operator<=>(const Xi_Stmts &lhs, const Xi_Stmts &rhs)
    -> std::partial_ordering
{
    return lhs.stmts <=> rhs.stmts;
}

auto TypeAssign(Xi_Stmts &stmt, LocalVariableRecord &) -> TypeAssignResult;

} // namespace xi
