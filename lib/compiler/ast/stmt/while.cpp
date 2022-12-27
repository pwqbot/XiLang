#include "compiler/ast/stmt/while.h"

#include "compiler/ast/all.h"
#include "compiler/ast/error.h"
#include "error.h"

namespace xi
{

auto TypeAssign(Xi_While &whilest, LocalVariableRecord record) -> TypeAssignResult
{
    return TypeAssign(whilest.cond, record) >>=
           [&whilest, &record](auto cond_type) -> TypeAssignResult
    {
        if (cond_type != type::buer{})
        {
            return tl::make_unexpected(TypeAssignError{
                TypeAssignError::TypeMismatch,
                fmt::format("expect buer, find {}", cond_type)});
        }
        return flatmap_(
                   whilest.body,
                   [&record](auto &x)
                   {
                       return TypeAssign(x, record);
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
