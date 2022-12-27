#pragma once

#include "compiler/ast/error.h"
#include "compiler/ast/expr/expr.h"
#include "compiler/ast/type.h"

#include <string>

namespace xi
{

struct Xi_Var
{
    std::string   name;
    Xi_Expr       value;
    std::string   type_name;
    type::Xi_Type type                              = type::unknown{};
    auto          operator<=>(const Xi_Var &) const = default;
};

auto TypeAssign(Xi_Var &, LocalVariableRecord&) -> TypeAssignResult;

} // namespace xi
