#include "compiler/ast/expr/array.h"

#include "compiler/ast/all.h"
#include "compiler/functional/monad.h"

namespace xi
{

auto TypeAssign(Xi_Array &arr, LocalVariableRecord record) -> TypeAssignResult
{
    return flatmap(
               arr.elements,
               [record](Xi_Expr &expr)
               {
                   return TypeAssign(expr, record);
               }
           ) >>=
           [&arr](std::vector<type::Xi_Type> member_types) -> TypeAssignResult
    {
        if (!ranges::equal(member_types, member_types))
        {
            return tl::make_unexpected(TypeAssignError{
                TypeAssignError::TypeMismatch,
                fmt::format("Array elements are not of the same type"),
            });
        }
        auto array_type = type::array{member_types.front()};
        return arr.type = array_type;
    };
}

} // namespace xi
