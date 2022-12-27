#pragma once

#include "compiler/ast/error.h"

#include <string>

namespace xi
{

struct Xi_Comment
{
    std::string text;
    auto        operator<=>(const Xi_Comment &rhs) const = default;
};

auto TypeAssign(Xi_Comment &stmt, LocalVariableRecord = {}) -> TypeAssignResult;

} // namespace xi
