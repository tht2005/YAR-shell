cmake_minimum_required(VERSION 3.16)

project(yar VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(__DEBUG)
else()
    add_compile_options(-O2 -march=native -flto)
    add_link_options(-flto)
endif()

add_compile_definitions(STRING_INIT_BUF=16)
# add_compile_definitions(_GNU_SOURCE)

find_package (PkgConfig REQUIRED)
pkg_check_modules (READLINE REQUIRED readline)

file(GLOB BUILTIN_CMD_SRCS "builtin_commands/*.c")
add_library(BuiltinCommands STATIC ${BUILTIN_CMD_SRCS})

file(GLOB DATA_STRUCTURE_SRCS "data_structure/*.c")
add_library(DataStructures STATIC ${DATA_STRUCTURE_SRCS})

set(LIBGLOB_SRCS "libglob/glob.c")
add_library(Glob STATIC ${LIBGLOB_SRCS})

# All libs
add_library(parent_libs INTERFACE)
target_link_libraries(parent_libs INTERFACE Program DataStructures BuiltinCommands Glob)

set(SRC_FILES   yar.c 
                yar_env.c
                yar_exec.c
                yar_job.c
                yar_shell.c
                yar_lexer.c
                yar_parser.tab.c
                yar_interpreter.c
                yar_ast.c
                yar_substitution.c
                yar_debug.c
            )
add_library (Program STATIC ${SRC_FILES})
add_executable(yar ${SRC_FILES})
target_link_options(yar PRIVATE
    "-Wl,--whole-archive"
    libBuiltinCommands.a
    "-Wl,--no-whole-archive"
)
target_link_libraries (
    yar     ${READLINE_LIBRARIES}
            parent_libs
)
target_include_directories(
    yar PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/lib
    ${READLINE_INCLUDE_DIRS}
)
target_compile_options (
    yar PUBLIC
    ${READLINE_CFLAGS}
    ${READLINE_LDFLAGS}
)

# Testing
enable_testing ()
add_subdirectory (tests)

install(TARGETS yar 
        RUNTIME DESTINATION bin)
