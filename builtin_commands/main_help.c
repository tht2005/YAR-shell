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
"Yar: Simple scripting language.\n"
"\n"
"Builtin Commands:\n"
"\thelp: Show help\n"
"\tls: show current directory's content\n"
"\tpwd: show current directory\n"
);
    fflush (stdout);
    return 0;
}

__attribute__((constructor))
static void __help_register (void)
{
    builtin_command_register("help", help_main, help_pre, help_post);
}

