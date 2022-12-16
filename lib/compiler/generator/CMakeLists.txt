file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp *.h)

find_package(LLVM CONFIG REQUIRED)
find_package(spdlog REQUIRED)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
# add_definitions(${LLVM_DEFINITIONS})

# Find the libraries that correspond to the LLVM components that we wish to use
llvm_map_components_to_libnames(llvm_libs core support irreader)
message(STATUS "LLVM libs: ${llvm_libs}")

add_library(generator STATIC ${srcs})
target_include_directories(generator PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(generator PUBLIC ${llvm_libs} spdlog::spdlog)
