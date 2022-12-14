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
        std::cout << "input: " << input << std::endl;
        if (input == "exit()")
        {
            return 0;
        }
        std::cout << "input: " << input << std::endl;
        total_input += input;

        auto total_result = xi::Xi_program(total_input);
        if (total_result)
        {
            if (total_result.value().second.empty())
            {
                fmt::print("Parse successfully\n");
                fmt::print("AST:\n {}\n", total_result.value());
            }
            else
            {
                fmt::print("Parse fail!\n {} not parsed", total_result.value().second);
            }
        }
        else
        {
            std::cout << "Failed to parse\n";
        }
    }
}
