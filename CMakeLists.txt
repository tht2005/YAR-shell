cmake_minimum_required(VERSION 3.16)

project(yar VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_definitions(STRING_INIT_BUF=16)

find_package (PkgConfig REQUIRED)
pkg_check_modules (READLINE REQUIRED readline)

set(SRC_FILES   yar.c 
                yar_parser.c
                yar_error.c
                data_structure/list_head.c
                data_structure/string.c
            )

add_executable(yar ${SRC_FILES})

target_link_libraries (
    yar
    ${READLINE_LIBRARIES}
)
target_include_directories(
    yar PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${READLINE_INCLUDE_DIRS}
)
target_compile_options (
    yar PUBLIC
    ${READLINE_CFLAGS}
    ${READLINE_LDFLAGS}
)

# configure_file(
#     include/config.h.in
#     include/config.h
#     @ONLY
# )
