#include "compiler/ast/expr/basic.h"

namespace xi
{

auto TypeAssign(Xi_Integer /*unused*/, LocalVariableRecord /*unused*/)
    -> TypeAssignResult
{
    return type::i64{};
}

auto TypeAssign(
    Xi_Real /*unused*/, LocalVariableRecord /*unused*/
) -> TypeAssignResult
{
    return type::real{};
}

auto TypeAssign(
    Xi_String /*unused*/, LocalVariableRecord /*unused*/
) -> TypeAssignResult
{
    return type::string{};
}

auto TypeAssign(
    Xi_Boolean /*unused*/, LocalVariableRecord /*unused*/
) -> TypeAssignResult
{
    return type::buer{};
}

} // namespace xi
