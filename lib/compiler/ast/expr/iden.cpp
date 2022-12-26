#include "compiler/ast/expr/iden.h"

#include "compiler/ast/all.h"
#include "compiler/ast/error.h"

namespace xi
{

auto TypeAssign(Xi_Iden &iden, LocalVariableRecord record) -> TypeAssignResult
{
    auto iden_type = record.find(iden.name);
    if (iden_type == record.end())
    {
        return tl::make_unexpected(TypeAssignError{
            TypeAssignError::UnknownVariable,
            fmt::format("undeclared variable {}", iden.name),
        });
    }
    return iden.type = iden_type->second;
}
} // namespace xi
