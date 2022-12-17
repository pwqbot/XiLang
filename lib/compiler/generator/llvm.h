#pragma once

#include "compiler/parser/utils.h"

#include <compiler/ast/ast.h>
#include <compiler/ast/ast_format.h>
#include <compiler/generator/error.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <map>
#include <spdlog/spdlog.h>
#include <tl/expected.hpp>

namespace xi
{
using codegen_result_t = ExpectedCodeGen<llvm::Value *>;

static std::unique_ptr<llvm::LLVMContext>   context;
static std::unique_ptr<llvm::IRBuilder<>>   builder;
static std::unique_ptr<llvm::Module>        module;
static std::map<std::string, llvm::Value *> namedValues;
constexpr int                               llvm_int_precision = 64;

inline void InitializeModule()
{
    static const std::string moduleName = "xi module";
    context                             = std::make_unique<llvm::LLVMContext>();
    module  = std::make_unique<llvm::Module>(moduleName, *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

auto CodeGen(Xi_Expr expr) -> codegen_result_t;

template <typename T>
auto CodeGen(recursive_wrapper<T> wrapper)
{
    return CodeGen(wrapper.get());
}

auto CodeGen(Xi_Real real) -> codegen_result_t
{
    return llvm::ConstantFP::get(*context, llvm::APFloat(real.value));
}

auto CodeGen(Xi_Integer integer) -> codegen_result_t
{
    llvm::APSInt x;
    return llvm::ConstantInt::get(
        *context,
        llvm::APInt(
            llvm_int_precision, static_cast<uint64_t>(integer.value), true
        )
    );
}

auto CodeGen(Xi_Boolean boolean) -> codegen_result_t
{
    return llvm::ConstantInt::get(
        *context, llvm::APInt(1, static_cast<uint64_t>(boolean.value))
    );
}

auto CodeGen(Xi_If if_expr) -> codegen_result_t
{
    return CodeGen(if_expr.cond) >>=
           [=](llvm::Value *cond_code) -> codegen_result_t
    {
        auto *function = builder->GetInsertBlock()->getParent();
        auto *then_bb  = llvm::BasicBlock::Create(*context, "then", function);
        auto *else_bb  = llvm::BasicBlock::Create(*context, "else");
        builder->CreateCondBr(cond_code, then_bb, else_bb);

        builder->SetInsertPoint(then_bb);
        return CodeGen(if_expr.then) >>=
               [&](llvm::Value *then_code) -> codegen_result_t
        {
            auto *merge_bb = llvm::BasicBlock::Create(*context, "ifcont");
            builder->CreateBr(merge_bb);
            then_bb = builder->GetInsertBlock();

            function->getBasicBlockList().push_back(else_bb);
            builder->SetInsertPoint(else_bb);

            return CodeGen(if_expr.els) >>=
                   [&](llvm::Value *else_code) -> codegen_result_t
            {
                builder->CreateBr(merge_bb);
                else_bb = builder->GetInsertBlock();

                function->getBasicBlockList().push_back(merge_bb);
                builder->SetInsertPoint(merge_bb);

                auto *phi =
                    builder->CreatePHI(then_code->getType(), 2, "iftmp");
                phi->addIncoming(then_code, then_bb);
                phi->addIncoming(else_code, else_bb);
                return phi;
            };
        };
    };
}

auto CodeGen(Xi_Iden iden) -> codegen_result_t
{
    auto *value = namedValues[iden.name];
    if (value == nullptr)
    {
        return tl::unexpected(
            ErrorCodeGen(ErrorCodeGen::UnknownVariable, iden.name)
        );
    }
    return value;
}

auto CodeGen(Xi_Binop bop) -> codegen_result_t
{
    return CodeGen(bop.lhs) >>= [bop](llvm::Value *lhs)
    {
        return CodeGen(bop.rhs) >>=
               [lhs, bop](llvm::Value *rhs) -> codegen_result_t
        {
            switch (bop.op)
            {
            case Xi_Op::Add:
                return builder->CreateAdd(lhs, rhs, "addtmp");
            case Xi_Op::Sub:
                return builder->CreateSub(lhs, rhs, "subtmp");
            case Xi_Op::Mul:
                return builder->CreateMul(lhs, rhs, "multmp");
            case Xi_Op::Div:
                return builder->CreateSDiv(lhs, rhs, "divtmp");
            case Xi_Op::Lt:
                return builder->CreateICmpULT(lhs, rhs, "cmptmp");
            case Xi_Op::Gt:
                return builder->CreateICmpUGT(lhs, rhs, "cmptmp");
            case Xi_Op::Eq:
                return builder->CreateICmpEQ(lhs, rhs, "cmptmp");
            case Xi_Op::Neq:
                return builder->CreateICmpNE(lhs, rhs, "cmptmp");
            case Xi_Op::Leq:
                return builder->CreateICmpULE(lhs, rhs, "cmptmp");
            case Xi_Op::Geq:
                return builder->CreateICmpUGE(lhs, rhs, "cmptmp");
            default:
                return tl::unexpected(ErrorCodeGen(
                    ErrorCodeGen::UnknownOperator, magic_enum::enum_name(bop.op)
                ));
            }
        };
    };
}

auto CodeGen(Xi_Unop uop) -> codegen_result_t
{
    return CodeGen(uop.expr) >>= [uop](auto expr_code) -> codegen_result_t
    {
        switch (uop.op)
        {
        case Xi_Op::Sub:
            return builder->CreateNeg(expr_code);
        case Xi_Op::Not:
            return builder->CreateNot(expr_code);
        default:
            return tl::unexpected(ErrorCodeGen(
                ErrorCodeGen::UnknownOperator, magic_enum::enum_name(uop.op)
            ));
        }
    };
}

auto CodeGen(Xi_Call call_expr) -> codegen_result_t
{
    llvm::Function *calleeF = module->getFunction(call_expr.name.name);
    if (calleeF == nullptr)
    {
        return tl::unexpected(
            ErrorCodeGen(ErrorCodeGen::UnknownFunction, call_expr.name.name)
        );
    }
    if (!calleeF->isVarArg() && calleeF->arg_size() != call_expr.args.size())
    {
        return tl::unexpected(ErrorCodeGen(
            ErrorCodeGen::InvalidArgumentCount,
            fmt::format(
                "call {}, want {}, get {}",
                calleeF->getName(),
                calleeF->arg_size(),
                call_expr.args.size()
            )
        ));
    }

    return flatmap(call_expr.args, [](auto arg) { return CodeGen(arg); }) >>=
           [calleeF](std::vector<llvm::Value *> argsV) -> codegen_result_t
    {
        return builder->CreateCall(calleeF, argsV, "calltmp");
    };
}

auto CodeGen(Xi_Lam) -> codegen_result_t
{
    return tl::unexpected(ErrorCodeGen(ErrorCodeGen::NotImplemented, "Lambda"));
}

auto XiTypeToLLVMType(Xi_Type xi_t) -> ExpectedCodeGen<llvm::Type *>
{
    switch (xi_t)
    {
    case Xi_Type::real:
        return llvm::Type::getDoubleTy(*context);
    case Xi_Type::i64:
        return llvm::Type::getInt64Ty(*context);
    case Xi_Type::string:
        return llvm::PointerType::get(llvm::Type::getInt8Ty(*context), 0);
    default:
        return tl::unexpected(
            ErrorCodeGen(ErrorCodeGen::UnknownType, magic_enum::enum_name(xi_t))
        );
    }
}

auto CodeGen(Xi_String s) -> codegen_result_t
{
    return builder->CreateGlobalStringPtr(s.value);
}

auto CodeGen(Xi_Decl decl) -> codegen_result_t
{
    return flatmap(decl.params_type, XiTypeToLLVMType) >>=
           [decl](auto arg_types) -> codegen_result_t
    {
        return XiTypeToLLVMType(decl.return_type) >>=
               [decl, arg_types](auto return_type) mutable -> codegen_result_t
        {
            auto *func_type =
                llvm::FunctionType::get(return_type, arg_types, decl.is_vararg);

            auto *function = llvm::Function::Create(
                func_type,
                llvm::Function::ExternalLinkage,
                static_cast<std::string>(decl.name),
                module.get()
            );

            return function;
        };
    };
}

auto CodeGen(Xi_Expr expr) -> codegen_result_t
{
    return std::visit([](auto expr_) { return CodeGen(expr_); }, expr);
}

auto checkFunc(Xi_Func xi) -> ExpectedCodeGen<llvm::Function *>
{
    auto *func = module->getFunction(static_cast<std::string>(xi.name));
    if (func == nullptr)
    {
        spdlog::error("Unknown function referenced");
        return tl::unexpected(
            ErrorCodeGen(ErrorCodeGen::UnknownFunction, xi.name.name)
        );
    }
    if (!func->empty())
    {
        return tl::unexpected(
            ErrorCodeGen(ErrorCodeGen::Redefinition, xi.name.name)
        );
    }
    if (!func->isVarArg() && xi.params.size() != func->arg_size())
    {
        return tl::unexpected(ErrorCodeGen(
            ErrorCodeGen::InvalidArgumentCount,
            fmt::format(
                "func {}, want {}, get {}",
                func->getName(),
                func->arg_size(),
                xi.params.size()
            )
        ));
    }
    return func;
}

auto CodeGen(Xi_Func xi) -> codegen_result_t
{
    return checkFunc(xi) >>= [xi](auto func) -> codegen_result_t
    {
        auto *bb = llvm::BasicBlock::Create(*context, "entry", func);
        builder->SetInsertPoint(bb);
        namedValues.clear();
        auto param_it = xi.params.begin();
        for (auto &arg : func->args())
        {
            arg.setName(param_it->name);
            param_it++;
            namedValues[arg.getName().str()] = &arg;
        }
        auto body = CodeGen(xi.expr);
        if (body)
        {
            builder->CreateRet(body.value());
            llvm::verifyFunction(*func);
            return func;
        }
        func->eraseFromParent();
        return body;
    };
}

auto CodeGen(Xi_Stmt stmt) -> codegen_result_t
{
    return std::visit([](auto stmt_) { return CodeGen(stmt_); }, stmt);
}

auto CodeGen(Xi_Program program) -> ExpectedCodeGen<std::string>
{
    InitializeModule();
    return flatmap(program.stmts, [](auto arg) { return CodeGen(arg); }) >>=
           [](auto) -> ExpectedCodeGen<std::string>
    {
        std::string              output;
        llvm::raw_string_ostream os(output);
        module->print(os, nullptr);
        return output;
    };
}

auto GenObj(std::string_view output_file)
{
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    auto TargetTriple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(TargetTriple);

    std::string Error;
    const auto *Target =
        llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (Target == nullptr)
    {
        llvm::errs() << Error;
        return 1;
    }

    const auto *CPU      = "generic";
    const auto *Features = "";

    llvm::TargetOptions opt;
    auto                RM = llvm::Optional<llvm::Reloc::Model>();
    auto               *TargetMachine =
        Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    module->setDataLayout(TargetMachine->createDataLayout());

    std::error_code      EC;
    llvm::raw_fd_ostream dest(output_file, EC, llvm::sys::fs::OF_None);

    if (EC)
    {
        llvm::errs() << "Could not open file: " << EC.message();
        return 1;
    }

    llvm::legacy::PassManager pass;
    auto                      FileType = llvm::CGFT_ObjectFile;

    if (TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType))
    {
        llvm::errs() << "TargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(*module);
    dest.flush();
    llvm::outs() << "Wrote " << output_file << "\n";
    return 1;
}
} // namespace xi
