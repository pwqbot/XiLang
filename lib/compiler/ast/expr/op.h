#pragma once

#include "compiler/ast/error.h"
#include "compiler/ast/expr/expr.h"
#include "compiler/ast/ast.h"
#include "compiler/ast/type.h"

namespace xi
{

// binary expression
struct Xi_Binop
{
    Xi_Expr       lhs;
    Xi_Expr       rhs;
    Xi_Op         op;
    type::Xi_Type type  = type::unknown{};
    int64_t       index = 0;
    auto          operator==(const Xi_Binop &b) const -> bool
    {
        return *this <=> b == nullptr;
    }
};

inline auto operator<=>(const Xi_Binop &lhs, const Xi_Binop &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.lhs <=> rhs.lhs; cmp != nullptr)
    {
        return cmp;
    }
    if (auto cmp = lhs.rhs <=> rhs.rhs; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.op <=> rhs.op;
}

struct Xi_Unop
{
    Xi_Expr       expr;
    Xi_Op         op;
    type::Xi_Type type = type::unknown{};
};

inline auto operator<=>(const Xi_Unop &lhs, const Xi_Unop &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.expr <=> rhs.expr; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.op <=> rhs.op;
}

auto typeAssignDot(Xi_Binop &binop, LocalVariableRecord record)
    -> TypeAssignResult;
auto TypeAssign(Xi_Binop &binop, LocalVariableRecord record)
    -> TypeAssignResult;
auto TypeAssign(Xi_Unop &unop, LocalVariableRecord record) -> TypeAssignResult;

} // namespace xi
