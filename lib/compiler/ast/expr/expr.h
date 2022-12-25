#pragma once

#include "compiler/ast/expr/basic.h"
#include "compiler/ast/error.h"

#include <compare>
#include <variant>
namespace xi
{

struct Xi_Binop;
auto operator<=>(const Xi_Binop &lhs, const Xi_Binop &rhs)
    -> std::partial_ordering;

struct Xi_ArrayIndex;
auto operator<=>(const Xi_ArrayIndex &lhs, const Xi_ArrayIndex &rhs)
    -> std::partial_ordering;

struct Xi_If;
auto operator<=>(const Xi_If &lhs, const Xi_If &rhs) -> std::partial_ordering;

struct Xi_Unop;
auto operator<=>(const Xi_Unop &lhs, const Xi_Unop &rhs)
    -> std::partial_ordering;

struct Xi_Lam;
auto operator<=>(const Xi_Lam &lhs, const Xi_Lam &rhs) -> std::partial_ordering;

struct Xi_Call;
auto operator<=>(const Xi_Call &lhs, const Xi_Call &rhs)
    -> std::partial_ordering;

struct Xi_Iden;
auto operator<=>(const Xi_Iden &lhs, const Xi_Iden &rhs)
    -> std::partial_ordering;

struct Xi_Array;
auto operator<=>(const Xi_Array &lhs, const Xi_Array &rhs)
    -> std::partial_ordering;

using Xi_Expr = std::variant<
    std::monostate,
    Xi_Integer,
    Xi_Boolean,
    Xi_Real,
    Xi_String,
    recursive_wrapper<Xi_ArrayIndex>,
    recursive_wrapper<Xi_Iden>,
    recursive_wrapper<Xi_Unop>,
    recursive_wrapper<Xi_Binop>,
    recursive_wrapper<Xi_If>,
    recursive_wrapper<Xi_Lam>,
    recursive_wrapper<Xi_Call>,
    recursive_wrapper<Xi_Array>>;

auto TypeAssign(
    Xi_Expr &expr, LocalVariableRecord record = LocalVariableRecord{}
) -> TypeAssignResult;

} // namespace xi
