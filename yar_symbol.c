#include "yar_symbol.h"
#include "yar_env.h"
#include "data_structure/string.h"

typedef enum symbol_type {

    VARIABLE,
    FUNCTION

} symbol_type;

typedef struct symbol {

    symbol_type type;
    void *data;

} symbol;

typedef struct variable_data {

    scope_environ_t *scope;
    string name;

} variable_data;

typedef struct function_data {



} function_data;
