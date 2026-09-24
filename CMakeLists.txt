cmake_minimum_required(VERSION 4.3)
project(CLua)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(--target=x86_64-w64-mingw32)
endif()
if(WIN32 AND MINGW)
    # Pass target to Clangd without breaking GCC
    add_compile_options(-Xclang --target=x86_64-w64-mingw32)
endif()

include_directories(src)
include_directories(src/common)
include_directories(src/common/language_processing)
include_directories(src/debugger)
include_directories(src/linear_allocator)
include_directories(src/parser)
include_directories(src/semantic_analyzer)


add_executable(CLua
        src/common/language_processing/base.hpp
        src/common/language_processing/node_base.hpp
        src/common/language_processing/node_handle.hpp
        src/common/language_processing/node_manager.hpp
        src/common/language_processing/parser.hpp
        src/common/language_processing/source.hpp
        src/common/base.hpp
        src/debugger/debugger.hpp
        src/linear_allocator/linear_allocator.cpp
        src/linear_allocator/linear_allocator.hpp
        src/parser/example_parser.cpp
        src/parser/parser.cpp
        src/parser/parser.hpp
        src/semantic_analyzer/semantic_analyzer.cpp
        src/semantic_analyzer/semantic_analyzer.hpp
        src/main.cpp)
