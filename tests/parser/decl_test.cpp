#include "compiler/ast/type.h"
#include "test_header.h"

#include <compiler/ast/ast.h>
#include <compiler/parser/decl.h>

namespace xi
{

TEST_CASE("Parser Xi_Decl", "[Xi_DeclExpr]")
{
    REQUIRE_THAT(
        Xi_decl("fn func :: i64"),
        AstNodeMatcher(
            Xi_Decl{
                Xi_Iden{"func"},
                "i64",
                {},
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_decl("fn func :: real -> ... -> i64"),
        AstNodeMatcher(
            Xi_Decl{
                Xi_Iden{"func"},
                "i64",
                {"real", "..."},
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_decl("fn func :: i64 -> i64 -> i64"),
        AstNodeMatcher(
            Xi_Decl{
                Xi_Iden{"func"},
                "i64",
                {
                    "i64",
                    "i64",
                },
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_decl("fn func:: i64 -> arr[arr[i64]]"),
        AstNodeMatcher(
            Xi_Decl{
                Xi_Iden{"func"},
                "arr[arr[i64]]",
                {"i64"},
            },
            ""
        )
    );
}

} // namespace xi
