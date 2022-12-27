#pragma once

#include "compiler/ast/error.h"
#include "compiler/ast/type.h"

#include <string>
#include <vector>

namespace xi
{

struct Xi_Set
{
    std::string                                      name;
    std::vector<std::pair<std::string, std::string>> members;
    type::Xi_Type                                    type = type::unknown{};
    auto operator<=>(const Xi_Set &) const                = default;
};

auto TypeAssign(Xi_Set &, LocalVariableRecord = {}) -> TypeAssignResult;

} // namespace xi
