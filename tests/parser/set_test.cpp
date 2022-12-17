#include "test_header.h"

#include <compiler/ast/ast.h>
#include <compiler/parser/set.h>

namespace xi
{

TEST_CASE("Parse Xi_Set", "[Xi_DeclExpr]")
{
    REQUIRE_THAT(
        Xi_set("set a = ( (x i64) (y i64) )"),
        AstNodeMatcher(
            Xi_Set{
                "a",
                {Xi_Iden{"x", Xi_Type(Xi_Type::i64)},
                 Xi_Iden{"y", Xi_Type(Xi_Type::i64)}},
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_set("set a = ( (x i64) (y i64) (z b) )"),
        AstNodeMatcher(
            Xi_Set{
                "a",
                {
                    Xi_Iden{"x", Xi_Type(Xi_Type::i64)},
                    Xi_Iden{"y", Xi_Type(Xi_Type::i64)},
                    Xi_Iden{"z", Xi_Type(Xi_Type::_set, "b")},
                },
            },
            ""
        )
    );
}

} // namespace xi
