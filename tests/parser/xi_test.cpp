#include "test_header.h"

#include <compiler/parser/xi.h>

namespace xi
{

TEST_CASE("Parse Xi_xi", "[Xi_Xi]")
{
    REQUIRE_THAT(
        Xi_xi("xi x = 1"),
        AstNodeMatcher(
            Xi_Xi{
                Xi_Iden{"x"},
                Xi_Integer{1},
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_xi("xi x = 1 + 3 + f(1 2 3)"),
        AstNodeMatcher(
            Xi_Xi{
                Xi_Iden{"x"},
                Xi_Binop{
                    Xi_Integer{1},
                    Xi_Binop{
                        Xi_Integer{3},
                        Xi_Call{
                            Xi_Iden{"f"},
                            {
                                Xi_Integer{1},
                                Xi_Integer{2},
                                Xi_Integer{3},
                            },
                        },
                        Xi_Op::Add,
                    },
                    Xi_Op::Add,
                },
            },
            ""
        )
    );
}

} // namespace xi
