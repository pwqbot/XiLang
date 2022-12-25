#include "compiler/ast/expr/array_index.h"

#include "compiler/ast/all.h"
#include "compiler/ast/ast_format.h"

namespace xi
{

auto TypeAssign(Xi_ArrayIndex &index, LocalVariableRecord record)
    -> TypeAssignResult
{
    auto tmpIden = Xi_Expr{Xi_Iden{
        .name = index.array_var_name,
        .expr = std::monostate{},
    }};
    return TypeAssign(tmpIden, record) >>=
           [&index, record](type::Xi_Type array_type_v)
    {
        return std::visit(
            [&index, record](auto array_type_wrapper) -> TypeAssignResult
            {
                if constexpr (std::same_as<
                                  std::decay_t<decltype(array_type_wrapper)>,
                                  recursive_wrapper<type::array>>)
                {
                    return TypeAssign(index.index, record) >>=
                           [&array_type_wrapper](type::Xi_Type index_type
                           ) -> TypeAssignResult
                    {
                        if (index_type != type::i64{})
                        {
                            return tl::make_unexpected(TypeAssignError{
                                TypeAssignError::TypeMismatch,
                                fmt::format(
                                    "array index must be i64, but got {}",
                                    index_type
                                ),
                            });
                        }
                        return array_type_wrapper.get().inner_type;
                    };
                }
                else
                {
                    return tl::make_unexpected(TypeAssignError{
                        TypeAssignError::TypeMismatch,
                        fmt::format("{}", array_type_wrapper),
                    });
                }
            },
            array_type_v
        );
    };
}
} // namespace xi
