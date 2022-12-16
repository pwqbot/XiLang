file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp *.h)

find_package(range-v3 REQUIRED)

add_library(functional INTERFACE ${srcs})
# target_include_directories(functional PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(functional INTERFACE ${llvm_libs} range-v3::range-v3)
