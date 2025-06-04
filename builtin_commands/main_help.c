#include "builtin_commands.h"

#include <stdio.h>

void help_pre ()
{
    // khoi tao?
}

void help_post ()
{
    // giai phong bo nho?
}

int help_main(int argc, char **argv)
{
    puts(
"== Tiny-shell ==\n"
"Built-in commands\n"
"  help\n"
"  exit <n>\n"
"  date <format>\n"
"  ls <dir>\n"
"  path [-a <dir> | -d <index>]\n"
"  echo <str> ... \n"
"  pwd\n"
"  source <file>\n"
"\n"
"Job control\n"
"  jobs\n"
"  fg <pgid>\n"
"  bg <pgid>\n"
"  suspend <pgid>\n"
"  kill <pgid>\n"
        );
    fflush (stdout);
    return 0;
}

__attribute__((constructor))
static void __help_register (void)
{
    builtin_command_register("help", help_main, help_pre, help_post);
}

