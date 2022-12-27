
#include "compiler/ast/stmt/program.h"

#include "compiler/ast/all.h"
#include "compiler/ast/type.h"

namespace xi
{

auto TypeAssign(Xi_Program &program)
    -> ExpectedTypeAssign<std::vector<type::Xi_Type>>
{
    LocalVariableRecord record;
    return flatmap_(
        program.stmts,
        [&record](Xi_Stmt &x)
        {
            fmt::print("record?? {}\n", record.size());
            return TypeAssign(x, record);
        }
    );
}

} // namespace xi
