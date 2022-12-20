#include "test_header.h"

#include <compiler/ast/ast.h>
#include <compiler/parser/array.h>

namespace xi
{

TEST_CASE("Parse Xi_Array", "[Xi_DeclExpr]")
{
    REQUIRE_THAT(
        Xi_array("[1, 2, 3]"),
        AstNodeMatcher(
            Xi_Array{
                .size = 3,
                .elements =
                    {
                        Xi_Integer{.value = 1},
                        Xi_Integer{.value = 2},
                        Xi_Integer{.value = 3},
                    },
            },
            ""
        )
    );

    auto empty_result = Xi_array("[]");
    REQUIRE(empty_result.has_value());
    REQUIRE_THAT(
        Xi_array("[  ]"),
        AstNodeMatcher(
            Xi_Array{
                .size     = 0,
                .elements = {},
            },
            ""
        )
    );
}

} // namespace xi
