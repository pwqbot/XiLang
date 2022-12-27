
#include "compiler/ast/stmt/comment.h"

#include "compiler/ast/error.h"

namespace xi
{

auto TypeAssign(Xi_Comment & /*unused*/, LocalVariableRecord) -> TypeAssignResult
{
    return xi::type::unknown{};
}
} // namespace xi
