#include "compiler/ast/stmt/while.h"
#include "compiler/ast/all.h"

#include "error.h"

namespace xi
{

auto TypeAssign(Xi_While & /*unused*/) -> TypeAssignResult
{
    return xi::type::unknown{};
}

} // namespace xi
