#include "test_header.h"

#include <compiler/parser/lam_expr.h>
#include <compiler/parser/math_expr.h>

namespace xi
{

// NOLINTBEGIN(cppcoreguidelines-*, readability*)
TEST_CASE("Parse Xi_Iden", "[Xi_Iden]")
{
    auto [iden1, iden2] = Xi_iden("abc").value();
    REQUIRE(iden1 == Xi_Iden{"abc"});
    REQUIRE(iden2 == "");

    auto [iden3, iden4] = Xi_iden("abc123").value();
    REQUIRE(iden3 == Xi_Iden{"abc123"});
    REQUIRE(iden4 == "");

    auto [iden5, iden6] = Xi_iden("abc_123").value();
    REQUIRE(iden5 == Xi_Iden{"abc_123"});
    REQUIRE(iden6 == "");

    auto [iden7, iden8] = Xi_iden("abc 123").value();
    REQUIRE(iden7 == Xi_Iden{"abc"});
    REQUIRE(iden8 == " 123");
}

// NOLINTEND(cppcoreguidelines-*, readability*)

} // namespace xi
