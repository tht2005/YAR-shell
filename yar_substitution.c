#include "yar_debug.h"
#include "yar_substitution.h"
#include "data_structure/string.h"

#include <stdio.h>
#include <regex.h>
#include <stdlib.h>


#define IDENTIFIER          "([a-zA-Z_][a-zA-Z0-9_]*)"
#define VALUE               "(.*?)"
// #define VALUE               "([^}]*)"

int __regex_match (const char *pattern, const char *input, size_t nmatch, regmatch_t *pmatch)
{
    regex_t reg;
    int ret;
    ret = regcomp(&reg, pattern, REG_EXTENDED);
    if (ret != 0) {
        return ret;
    }
    ret = regexec(&reg, input, nmatch, pmatch, 0);
    regfree(&reg);
    return ret;
}

string __get_capture (const char *input, regmatch_t pmatch)
{
    int start = pmatch.rm_so;
    int end = pmatch.rm_eo;
    return new_substr (input + start, input + end);
}

string __get_environment_variable (const char *name)
{
    return new_string_2 (getenv (name));
}

// ${IDENTIFIER}
string variable_substitution_handler (const char *input)
{
    // skip first character ( $ )
    return __get_environment_variable (input + 1);
}

string string_substitution_handler (const char *input)
{
    regmatch_t pmatch[16];
    string result = NULL;

    // ${VAR}
    if (__regex_match("\\$\\{" IDENTIFIER "\\}", input, 2, pmatch) == 0)
    {
        string name = __get_capture(input, pmatch[1]);
        DEBUG_PRINT ("debug: variable substitution: `%s`\n", name);
        result = __get_environment_variable (name);
        free_string(name);
    }
    // default value (if unset or null) ${VAR:-word}
    else if (__regex_match("\\$\\{" IDENTIFIER ":-" VALUE "\\}", input, 3, pmatch) == 0)
    {
        string name = __get_capture (input, pmatch[1]);
        string word = __get_capture (input, pmatch[2]);
        DEBUG_PRINT ("debug: return default value if unset or null, `%s` `%s`\n", name, word);
        result = new_string_2("not_implement_yet");
        free_string(name);
        free_string(word);
    }
    // default value (if unset) ${VAR-word}
    else if (__regex_match("\\$\\{" IDENTIFIER "-" VALUE "\\}", input, 3, pmatch) == 0)
    {
        string name = __get_capture (input, pmatch[1]);
        string word = __get_capture (input, pmatch[2]);
        DEBUG_PRINT ("debug: return default value if unset, `%s` `%s`\n", name, word);
        result = new_string_2("not_implement_yet");
        free_string(name);
        free_string(word);
    }
    // assign default if unset or null ${VAR:=word}
    else if (__regex_match("\\$\\{" IDENTIFIER ":=" VALUE "\\}", input, 3, pmatch) == 0)
    {
        string name = __get_capture (input, pmatch[1]);
        string word = __get_capture (input, pmatch[2]);
        DEBUG_PRINT ("debug: assign default value if unset or null, `%s` `%s`\n", name, word);
        result = new_string_2("not_implement_yet");
        free_string(name);
        free_string(word);
    }
    // error message if unset or null ${VAR:?message}
    else if (__regex_match("\\$\\{" IDENTIFIER ":\\?" VALUE "\\}", input, 3, pmatch) == 0)
    {
        string name = __get_capture (input, pmatch[1]);
        string message = __get_capture (input, pmatch[2]);
        DEBUG_PRINT ("debug: error message if unset or null, `%s` `%s`\n", name, message);
        result =new_string_2("not_implement_yet");
        free_string(name);
        free_string(message);
    }
    else {
        fprintf (stderr, "Yar: Bad substitution: %s\n", input);
        result = new_string_2 ("bad_substitution");
    }
    return result;
}
