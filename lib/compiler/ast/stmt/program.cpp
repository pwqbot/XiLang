
#include "compiler/ast/stmt/program.h"
#include "compiler/ast/all.h"

namespace xi
{

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
