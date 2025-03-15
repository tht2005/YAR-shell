cmake_minimum_required(VERSION 3.16)

project(yar VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_FILES   main.c 
                yar_lexer.c
                yar_literal.c
                yar_token.c
                data_structure/list_head.c
                data_structure/string.c
            )

add_executable(yar ${SRC_FILES})

target_include_directories(
    yar PUBLIC
    ${PROJECT_SOURCE_DIR}
)

# configure_file(
#     include/config.h.in
#     include/config.h
#     @ONLY
# )
