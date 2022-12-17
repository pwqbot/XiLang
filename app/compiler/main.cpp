#include <compiler/ast/ast_format.h>
#include <compiler/generator/llvm.h>
#include <compiler/parser/program.h>

int main()
{
    std::string_view input_file = "app/compiler/test.xi";
    std::string_view output_obj = "app/compiler/test.o";
    std::string_view output_ir  = "app/compiler/test.ir";
    std::string      input_string;
    // read the whole file into a string
    std::ifstream file(input_file.data());
    input_string.assign(
        std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()
    );
    fmt::print("Input file: {}\n", input_string);

    auto ast_result = xi::Xi_program(input_string);
    if (ast_result)
    {
        if (ast_result.value().second == "\n")
        {
            fmt::print("Parse successfully\n");
            fmt::print("AST:\n {}\n", ast_result.value().first);
            auto codegen_result = xi::CodeGen(ast_result.value().first);
            if (!codegen_result)
            {
                fmt::print("LLVM: \n {}\n", codegen_result.error().what());
                return 1;
            }
            // write the IR to a file
            std::ofstream ir_file(output_ir.data());
            ir_file << codegen_result.value();

            xi::GenObj(output_obj);
        }
        else
        {
            fmt::print(
                "Parse fail!\nparsed: {}\n{} not parsed\n",
                ast_result.value().first,
                ast_result.value().second
            );
        }
    }
    else
    {
        fmt::print("Failed to parse\n");
        return 1;
    }
    return 0;
}
