#include "compiler/ast/stmt/while.h"

#include "compiler/ast/all.h"
#include "error.h"

namespace xi
{

auto TypeAssign(Xi_While &whilest) -> TypeAssignResult
{
    return TypeAssign(whilest.cond) >>=
           [&whilest](auto cond_type) -> TypeAssignResult
    {
        if (cond_type != type::buer{})
        {
            return tl::make_unexpected(TypeAssignError{
                TypeAssignError::TypeMismatch,
                fmt::format("expect buer, find {}", cond_type)});
        }
        return flatmap_(
                   whilest.body,
                   [](auto &x)
                   {
                       return TypeAssign(x);
                   }
               ) >>= [&whilest](std::vector<type::Xi_Type> body_types
                     ) -> TypeAssignResult
        {
            return whilest.type = type::types{
                       body_types,
                   };
        };
    };
}

} // namespace xi
