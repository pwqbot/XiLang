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
                Xi_Type(Xi_Type::i64),
                {},
                false,
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_decl("fn func :: real -> ... -> i64"),
        AstNodeMatcher(
            Xi_Decl{
                Xi_Iden{"func"},
                Xi_Type(Xi_Type::i64),
                {Xi_Type(Xi_Type::real)},
                true,
            },
            ""
        )
    );
}

} // namespace xi
