#pragma once

#include "compiler/ast/error.h"
#include "compiler/ast/expr/expr.h"
#include "compiler/ast/type.h"

#include <compare>
#include <string>
namespace xi
{

struct Xi_Iden
{
    std::string   name;
    Xi_Expr       expr;
    type::Xi_Type type = type::unknown{};
                  operator std::string() const { return name; }
    auto          operator==(const Xi_Iden &b) const -> bool
    {
        return *this <=> b == nullptr;
    }
};

inline auto operator<=>(const Xi_Iden &lhs, const Xi_Iden &rhs)
    -> std::partial_ordering
{
    if (auto cmp = lhs.name <=> rhs.name; cmp != nullptr)
    {
        return cmp;
    }
    if (auto cmp = lhs.expr <=> rhs.expr; cmp != nullptr)
    {
        return cmp;
    }
    return lhs.type <=> rhs.type;
}

auto TypeAssign(Xi_Iden &iden, LocalVariableRecord record) -> TypeAssignResult;

} // namespace xi
