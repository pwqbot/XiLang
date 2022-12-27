#include "compiler/parser/program.h"
#include "test_header.h"

namespace xi
{

TEST_CASE("Parse var")
{
    REQUIRE_THAT(
        Xi_var("int x;"),
        AstNodeMatcher(Xi_Stmt{
            Xi_Var{
                .name      = "x",
                .value     = Xi_Expr{std::monostate{}},
                .type_name = "int",
            },
        })
    );

    REQUIRE_THAT(
        Xi_var("int x = 1;"),
        AstNodeMatcher(Xi_Stmt{
            Xi_Var{
                .name      = "x",
                .value     = Xi_Expr{Xi_Integer{1}},
                .type_name = "int",
            },
        })
    );
}

} // namespace xi
