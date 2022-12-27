#pragma once

#include "compiler/ast/error.h"
#include "compiler/ast/type.h"

#include <string>
#include <vector>

namespace xi
{

struct Xi_Decl
{
    std::string              name;
    std::string              return_type;
    std::vector<std::string> params_type;
    bool                     is_vararg = false;
    type::Xi_Type            type      = type::unknown{};
    auto                     operator<=>(const Xi_Decl &) const = default;
};

auto TypeAssign(Xi_Decl &decl, LocalVariableRecord = {}) -> TypeAssignResult;

} // namespace xi
