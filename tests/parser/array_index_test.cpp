#include "test_header.h"

#include <compiler/ast/ast.h>
#include <compiler/parser/array_index.h>

namespace xi
{

TEST_CASE("Parse Xi_ArrayIndex", "[Xi_DeclExpr]")
{
    REQUIRE_THAT(
        Xi_arrayIndex("a[1]"),
        AstNodeMatcher(
            Xi_ArrayIndex{
                .array_var_name = "a",
                .index          = Xi_Integer{1},
            },
            ""
        )
    );
}

} // namespace xi
