#include "test_header.h"

#include <compiler/generator/llvm.h>


namespace xi
{
TEST_CASE("Generate real") {
    // test llvm code generation
    auto codeGen = CodeGen(Xi_Real{1.0}).value();
    REQUIRE(codeGen != nullptr);
}
}
