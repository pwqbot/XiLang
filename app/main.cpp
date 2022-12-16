#include <compiler/ast/ast_format.h>
#include <compiler/generator/llvm.h>
#include <compiler/parser/program.h>
#include <iostream>

int main()
{
    std::string total_input;
    while (true)
    {
        std::cout << "xi> ";

        std::string input;
        std::getline(std::cin, input);
        if (std::cin.good())
        {
            input += '\n';
        }
        if (input == "exit()")
        {
            return 0;
        }
        total_input += input;

        auto total_result = xi::Xi_program(total_input);
        if (total_result)
        {
            if (total_result.value().second == "\n")
            {
                fmt::print("Parse successfully\n");
                fmt::print("AST:\n {}\n", total_result.value().first);
                fmt::print(
                    "LLVM IR:\n {}\n", xi::CodeGen(total_result.value().first)
                );
            }
            else
            {
                fmt::print(
                    "Parse fail!\n {} not parsed\n", total_result.value().second
                );
            }
        }
        else
        {
            fmt::print("Failed to parse\n");
        }
    }
}
