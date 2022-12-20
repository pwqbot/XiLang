#include <compiler/ast/ast_format.h>
#include <compiler/ast/type_assign.h>
#include <compiler/ast/type_format.h>
#include <compiler/generator/llvm.h>
#include <compiler/parser/program.h>
#include <cstdlib>
#include <gflags/gflags.h>
#include <spdlog/spdlog.h>

DEFINE_string(o, "a", "Output file");

int main(int argc, char *argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    if (argc < 2)
    {
        spdlog::error("No input file specified");
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_obj = FLAGS_o + ".o";
    std::string output_ll  = FLAGS_o + ".ll";

    std::string input_string;
    // read the whole file into a string
    std::ifstream file(input_file);
    input_string.assign(
        std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()
    );
    fmt::print("Input file: {}\n", input_string);

    auto ast_result = xi::Xi_program(input_string);
    if (ast_result)
    {
        if (ast_result.value().second == "\n")
        {
            spdlog::info("Parse successfully\n");
            spdlog::info("AST:\n {}\n", ast_result.value().first);
            auto ast      = ast_result.value().first;
            auto ast_type = TypeAssign(ast);
            if (!ast_type.has_value())
            {
                spdlog::error(
                    "Type error {}\n {}\n",
                    ast_type.error().what(),
                    ast_type.error().node
                );
                return 1;
            }

            spdlog::info("Type:\n {}\n", ast_type.value());
            auto codegen_result = xi::CodeGen(ast);
            if (!codegen_result)
            {
                spdlog::error("LLVM: \n {}\n", codegen_result.error().what());
                return 1;
            }
            // write the IR to a file
            spdlog::info("Generate LLVM IR to {}", output_ll);
            std::ofstream ir_file(output_ll.data());
            ir_file << codegen_result.value();

            xi::GenObj(output_obj);

            // use clang to link the object file
            spdlog::info("Linking to {}", FLAGS_o);
            auto clang_result = std::system(
                fmt::format("clang {} -o {}", output_obj, FLAGS_o).c_str()
            );
            if (clang_result != 0)
            {
                // delete the object file
                std::remove(output_obj.data());
                spdlog::error("Linking failed");
                return 1;
            }
            std::remove(output_obj.data());
            spdlog::info("Linking successfully");
        }
        else
        {
            spdlog::error(
                "Parse fail!\nparsed: {}\n{} not parsed\n",
                ast_result.value().first,
                ast_result.value().second
            );
        }
    }
    else
    {
        spdlog::error("Failed to parse\n");
        return 1;
    }
    return 0;
}
