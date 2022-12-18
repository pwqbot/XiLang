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
                type::i64{},
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
                type::i64{},
                {type::real{}},
                true,
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_decl("fn func :: i64 -> i64 -> i64"),
        AstNodeMatcher(
            Xi_Decl{
                Xi_Iden{"func"},
                type::i64{},
                {
                    type::i64{},
                    type::i64{},
                },
                false,
            },
            ""
        )
    );
}

} // namespace xi
