#pragma once

#include "compiler/ast/error.h"
#include "compiler/ast/expr/expr.h"
#include "compiler/ast/expr/iden.h"
#include "compiler/ast/stmt/stmt.h"

#include <string>
#include <vector>

namespace xi
{

struct Xi_Func
{
    std::string              name;
    std::vector<std::string> params;
    Xi_Expr                  expr;
    type::Xi_Type            type      = type::unknown{};
    std::vector<Xi_Iden>     let_idens = {};
    std::vector<Xi_Stmt>     stmts     = {};
};

inline auto operator<=>(const Xi_Func &lhs, const Xi_Func &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.name <=> rhs.name; cmp != nullptr)
    {
        return cmp;
    }
    if (auto cmp = lhs.params <=> rhs.params; cmp != nullptr)
    {
        return cmp;
    }
    if (auto cmp = lhs.expr <=> rhs.expr; cmp != nullptr)
    {
        return cmp;
    }
    if (auto cmp = lhs.type <=> rhs.type; cmp != nullptr)
    {
        return cmp;
    }
    if (auto cmp = lhs.let_idens <=> rhs.let_idens; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.stmts <=> rhs.stmts;
}

auto TypeAssign(Xi_Func &func_def) -> TypeAssignResult;

} // namespace xi
