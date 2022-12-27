
#include "compiler/ast/stmt/var.h"

#include "compiler/ast/error.h"

namespace xi
{

auto TypeAssign(Xi_Var &var, LocalVariableRecord &record) -> TypeAssignResult
{
    return findTypeInSymbolTable(var.type_name, SymbolType::Type) >>=
           [&record, &var](type::Xi_Type expect_type) -> TypeAssignResult
    {
        return TypeAssign(var.value, record) >>=
               [&expect_type, &var, &record](type::Xi_Type expr_type
               ) -> TypeAssignResult
        {
            if (expr_type != expect_type)
            {
                return tl::make_unexpected(TypeAssignError{
                    TypeAssignError::TypeMismatch,
                    fmt::format(
                        "variable declare as {}, but its value is {}",
                        expect_type,
                        expr_type
                    )});
            }
            record.insert_or_assign(var.name, expect_type);
            return var.type = expect_type;
        };
    };
}

} // namespace xi
