#include "compiler/ast/expr/lam.h"

#include "compiler/ast/all.h"

namespace xi
{

auto TypeAssign(Xi_Lam & /*unused*/, LocalVariableRecord /*unused*/)
    -> TypeAssignResult
{
    return tl::make_unexpected(TypeAssignError{
        TypeAssignError::TypeMismatch, "not implemented",});
}
} // namespace xi
