
#include "compiler/ast/stmt.h"
#include "compiler/ast/type.h"
#include "compiler/ast/type_assign.h"

#include <vector>

namespace xi
{

struct Xi_Program
{
    std::vector<Xi_Stmt> stmts;
    auto                 operator<=>(const Xi_Program &rhs) const = default;
};

auto TypeAssign(Xi_Program &program)
    -> ExpectedTypeAssign<std::vector<type::Xi_Type>>
{
    return flatmap_(
        program.stmts,
        [](auto &x)
        {
            return TypeAssign(x);
        }
    );
}


} // namespace xi
