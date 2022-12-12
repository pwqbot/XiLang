#include "test_header.h"

#include <compiler/parser/if_expr.h>

namespace xi
{

TEST_CASE("Parse Xi_If", "[Xi_If]")
{
    auto [if1, if2] = Xi_if("if true then 1 else 2").value();
    REQUIRE(
        if1 ==
        Xi_If{
            Xi_Boolean{true},
            Xi_Integer{1},
            Xi_Integer{2},
        }
    );
    REQUIRE(if2.empty());

    auto [if3, if4] = Xi_if("if true || false then 1 + 2 else 3 * 4 ").value();
    REQUIRE(
        if3 ==
        Xi_If{
            Xi_Binop{
                Xi_Boolean{true},
                Xi_Boolean{false},
                Xi_Op::Or,
            },
            Xi_Binop{
                Xi_Integer{1},
                Xi_Integer{2},
                Xi_Op::Add,
            },
            Xi_Binop{
                Xi_Integer{3},
                Xi_Integer{4},
                Xi_Op::Mul,
            },
        }
    );
}

} // namespace xi
