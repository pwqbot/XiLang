#include "test_header.h"

#include <compiler/parser/func.h>

namespace xi
{

TEST_CASE("Parse Xi_func", "[Xi_Xi]")
{
    REQUIRE_THAT(
        Xi_func("x = 1"),
        AstNodeMatcher(
            Xi_Func{
                Xi_Iden{"x"},
                {},
                Xi_Integer{1},
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_func("x = 1 + 3 + f(1 2 3)"),
        AstNodeMatcher(
            Xi_Func{
                Xi_Iden{"x"},
                {},
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

    REQUIRE_THAT(
        Xi_func("func x y = x + y + 1"),
        AstNodeMatcher(
            Xi_Func{
                Xi_Iden{"func"},
                {
                    Xi_Iden{"x"},
                    Xi_Iden{"y"},
                },
                Xi_Binop{
                    Xi_Iden{"x"},
                    Xi_Binop{
                        Xi_Iden{"y"},
                        Xi_Integer{1},
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
