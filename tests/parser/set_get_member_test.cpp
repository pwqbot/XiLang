#include "test_header.h"

#include <compiler/ast/ast.h>
#include <compiler/parser/set_get_member.h>

namespace xi
{

TEST_CASE("Parse Xi_SetGetM", "[Xi_DeclExpr]")
{
    REQUIRE_THAT(
        Xi_setGetM("a.x"),
        AstNodeMatcher(
            Xi_SetGetM{
                .set_var_name = "a",
                .member_name  = "x",
            }
        ,"")
    );
}

} // namespace xi
