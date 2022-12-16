file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp *.h)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

message(STATUS "LLVM libs: ${llvm_libs}")

add_library(functional INTERFACE ${srcs})
# target_include_directories(functional PRIVATE ${LLVM_INCLUDE_DIRS})
# target_link_libraries(functional PUBLIC ${llvm_libs} spdlog::spdlog
                                        # tl::expected)
