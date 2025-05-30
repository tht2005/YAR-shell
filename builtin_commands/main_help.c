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
"Yar: Simple scripting language."

"Builtin Commands:"
"\thelp: Show help"
"\tls: show current directory's content"
"\tpwd: show current directory"
);
    fflush (stdout);
    return 0;
}

__attribute__((constructor))
static void __help_register (void)
{
    builtin_command_register("help", CHILD_PROCESS, help_main, help_pre, help_post);
}

