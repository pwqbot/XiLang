#pragma once

#include <compiler/ast/ast.h>
#include <compiler/ast/ast_format.h>
#include <compiler/ast/type.h>
#include <compiler/generator/error.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/utils.h>
#include <compiler/util/expected.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Intrinsics.h>
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
#include <variant>

namespace xi
{
using codegen_result_t = ExpectedCodeGen<llvm::Value *>;

static std::unique_ptr<llvm::LLVMContext>   context;
static std::unique_ptr<llvm::IRBuilder<>>   builder;
static std::unique_ptr<llvm::Module>        module;
static std::unique_ptr<llvm::DataLayout>    dataLayout;
static std::map<std::string, llvm::Value *> namedValues;
constexpr int                               llvm_int_precision = 64;

inline void InitializeModule()
{
    static const std::string moduleName = "xi module";
    context                             = std::make_unique<llvm::LLVMContext>();
    module     = std::make_unique<llvm::Module>(moduleName, *context);
    builder    = std::make_unique<llvm::IRBuilder<>>(*context);
    dataLayout = std::make_unique<llvm::DataLayout>(module->getDataLayout());
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

auto XiTypeToLLVMType(type::Xi_Type xi_t) -> ExpectedCodeGen<llvm::Type *>
{
    return std::visit(
        []<typename T>(T t) -> ExpectedCodeGen<llvm::Type *>
        {
            if constexpr (std::same_as<T, type::real>)
            {
                return llvm::Type::getDoubleTy(*context);
            }
            else if constexpr (std::same_as<T, type::i64>)
            {
                return llvm::Type::getInt64Ty(*context);
            }
            else if constexpr (std::same_as<T, type::string>)
            {
                return llvm::PointerType::get(
                    llvm::Type::getInt8Ty(*context), 0
                );
            }
            else if constexpr (std::same_as<T, type::buer>)
            {
                return llvm::PointerType::get(
                    llvm::Type::getInt1Ty(*context), 0
                );
            }
            else if constexpr (std::same_as<T, recursive_wrapper<type::set>>)
            {
                return llvm::StructType::getTypeByName(*context, t.get().name);
            }
            else if constexpr (std::same_as<T, recursive_wrapper<type::array>>)
            {
                return XiTypeToLLVMType(t.get().inner_type) >>=
                       [](auto inner_type) -> ExpectedCodeGen<llvm::Type *>
                {
                    llvm::Type *intType = llvm::Type::getInt32Ty(*context);

                    std::vector<llvm::Type *> types = {
                        llvm::PointerType::get(inner_type, 0),
                        intType,
                    };
                    return llvm::StructType::get(*context, types);
                };
            }
            return tl::unexpected(
                ErrorCodeGen(ErrorCodeGen::UnknownType, fmt::format("{}", t))
            );
        },
        xi_t
    );
}

auto getArrayMemberType(Xi_Array arr) -> ExpectedCodeGen<llvm::Type *>
{
    return std::visit(
        [](auto arr_)
        {
            if constexpr (std::same_as<decltype(arr_), Xi_Array>)
            {
                return XiTypeToLLVMType(arr_.inner_type);
            }
            return tl::unexpected(
                ErrorCodeGen(ErrorCodeGen::UnknownType, "Unknown type")
            );
        },
        arr.type
    );
}

auto CodeGen(Xi_Array arr) -> codegen_result_t
{
    return getArrayMemberType(arr) >>=
           [arr](llvm::Type *element_type) -> codegen_result_t
    {
        auto *int32type = llvm::Type::getInt32Ty(*context);

        auto *element_size = llvm::ConstantInt::get(
            int32type, dataLayout->getTypeSizeInBits(element_type) / 8
        );
        // TODO(ding.wang): get size in run time
        auto *alloc_size = llvm::ConstantExpr::getMul(
            llvm::ConstantInt::get(int32type, arr.elements.size()), element_size
        );
        auto *arr_code = llvm::CallInst::CreateMalloc(
            builder->GetInsertBlock(),
            element_type->getPointerTo(),
            element_type,
            alloc_size,
            nullptr,
            nullptr,
            ""
        );
        return builder->Insert(arr_code);
    };
}

// generate user defined type
auto CodeGen(Xi_Set set) -> codegen_result_t
{
    auto set_type     = std::get<recursive_wrapper<type::set>>(set.type).get();
    auto member_types = set_type.members |
                        ranges::views::transform(
                            [](auto pair)
                            {
                                return pair.second;
                            }
                        ) |
                        ranges::to_vector;
    return flatmap(member_types, XiTypeToLLVMType) >>=
           [set](std::vector<llvm::Type *> llvm_members_type
           ) -> codegen_result_t
    {
        auto *struct_type =
            llvm::StructType::create(*context, llvm_members_type, set.name);
        // generate constructor
        auto *constructor = llvm::Function::Create(
            llvm::FunctionType::get(struct_type, llvm_members_type, false),
            llvm::Function::ExternalLinkage,
            set.name,
            module.get()
        );

        auto *entry = llvm::BasicBlock::Create(*context, "entry", constructor);
        builder->SetInsertPoint(entry);
        // create struct in stack
        auto *struct_ptr = builder->CreateAlloca(struct_type);

        unsigned int i = 0;
        for (auto &arg : constructor->args())
        {
            auto *field = builder->CreateStructGEP(struct_type, struct_ptr, i);
            i += 1;
            builder->CreateStore(&arg, field);
        }
        // return struct
        llvm::Value *struct_val = builder->CreateLoad(struct_type, struct_ptr);
        builder->CreateRet(struct_val);

        return struct_val;
    };
}

auto CodeGen(Xi_SetGetM set_get_m) -> codegen_result_t
{
    auto struct_pointer = namedValues.find(set_get_m.set_var_name);
    if (struct_pointer == namedValues.end())
    {
        return tl::unexpected(ErrorCodeGen(
            ErrorCodeGen::UnknownVariable,
            fmt::format("Unknown variable {}", set_get_m.set_var_name)
        ));
    }

    auto *member_index = llvm::ConstantInt::get(
        *context, llvm::APInt(32, set_get_m.member_index, true)
    );
    std::vector<llvm::Value *> indices(2);
    indices[0] = llvm::ConstantInt::get(*context, llvm::APInt(32, 0, true));
    indices[1] = member_index;
    return builder->CreateExtractValue(
        struct_pointer->second, static_cast<unsigned int>(set_get_m.member_index), "membertmp"
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
            // TODO(ding.wang): check type
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
        case Xi_Op::Add:
            return expr_code;
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
    llvm::Function *calleeF = module->getFunction(call_expr.name);

    return flatmap(
               call_expr.args,
               [](auto arg)
               {
                   return CodeGen(arg);
               }
           ) >>= [calleeF](std::vector<llvm::Value *> argsV) -> codegen_result_t
    {
        return builder->CreateCall(calleeF, argsV, "calltmp");
    };
}

auto CodeGen(Xi_Lam) -> codegen_result_t
{
    return tl::unexpected(ErrorCodeGen(ErrorCodeGen::NotImplemented, "Lambda"));
}

auto CodeGen(Xi_String s) -> codegen_result_t
{
    return builder->CreateGlobalStringPtr(s.value);
}

auto CodeGen(Xi_Decl decl) -> codegen_result_t
{
    return std::visit(
        [decl](auto decl_type_wrapper) -> codegen_result_t
        {
            if constexpr (std::same_as<
                              std::decay_t<decltype(decl_type_wrapper)>,
                              recursive_wrapper<type::function>>)
            {
                auto decl_type = decl_type_wrapper.get();
                return flatmap(decl_type.param_types, XiTypeToLLVMType) >>=
                       [decl, decl_type](auto arg_types) -> codegen_result_t
                {
                    return XiTypeToLLVMType(decl_type.return_type) >>=
                           [decl, arg_types, decl_type](llvm::Type *return_type
                           ) mutable -> codegen_result_t
                    {
                        auto *func_type = llvm::FunctionType::get(
                            return_type, arg_types, decl_type.is_vararg
                        );

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
            else
            {
                return tl::unexpected(ErrorCodeGen(
                    ErrorCodeGen::TypeMismatch,
                    fmt::format("{}", decl_type_wrapper)
                ));
            }
        },
        decl.type
    );
}

auto CodeGen(Xi_Expr expr) -> codegen_result_t
{
    return std::visit(
        [](auto expr_)
        {
            return CodeGen(expr_);
        },
        expr
    );
}

auto CodeGen(Xi_Func xi_func) -> codegen_result_t
{
    auto *llvm_func =
        module->getFunction(static_cast<std::string>(xi_func.name));

    auto *bb = llvm::BasicBlock::Create(*context, "entry", llvm_func);
    builder->SetInsertPoint(bb);

    namedValues.clear();
    auto param_it = xi_func.params.begin();
    for (auto &arg : llvm_func->args())
    {
        arg.setName(*param_it);
        param_it++;
        namedValues[arg.getName().str()] = &arg;
    }

    return flatmap(
               xi_func.let_idens,
               [](Xi_Iden iden)
               {
                   return CodeGen(iden.expr);
               }
           ) >>= [&llvm_func, xi_func](auto idens_code) -> codegen_result_t
    {
        for (const auto &[iden_code, let_var] :
             ranges::views::zip(idens_code, xi_func.let_idens))
        {
            namedValues[let_var.name] = iden_code;
        }

        auto body = CodeGen(xi_func.expr);
        if (body)
        {
            builder->CreateRet(body.value());
            llvm::verifyFunction(*llvm_func);
            return llvm_func;
        }

        llvm_func->eraseFromParent();
        return body;
    };
}

auto CodeGen(Xi_Stmt stmt) -> codegen_result_t
{
    return std::visit(
        [](auto stmt_)
        {
            return CodeGen(stmt_);
        },
        stmt
    );
}

auto CodeGen(Xi_Program program) -> ExpectedCodeGen<std::string>
{
    InitializeModule();
    return flatmap(
               program.stmts,
               [](auto arg)
               {
                   return CodeGen(arg);
               }
           ) >>= [](auto) -> ExpectedCodeGen<std::string>
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
