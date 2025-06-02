#include "builtin_commands.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

void sleep_pre ()
{
    // khoi tao?
}

void sleep_post ()
{
    // giai phong bo nho?
}

double parse_duration(const char *arg) {
    char *end;
    double value = strtod(arg, &end);
    if (end == arg || *(end + 1) != '\0') {
        return -1.0;
    }

    char suffix = *end;
    switch (suffix) {
        case '\0':
        case 's':
            return value;
        case 'm':
            return value * 60;
        case 'h':
            return value * 3600;
        case 'd':
            return value * 86400;
        default:
            return -1.0;
    }
}

int sleep_main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: sleep NUMBER[SUFFIX]...\n");
        printf("   or:  sleep OPTION\n");
        printf("Pause for NUMBER seconds.  SUFFIX may be 's' for seconds (the default),\n");
        printf("'m' for minutes, 'h' for hours or 'd' for days.  NUMBER need not be an\n");
        printf("integer.  Given two or more arguments, pause for the amount of time\n");
        printf("specified by the sum of their values.\n\n");
        fflush(stdout);
        return 0;
    }
    if (argc == 1) {
        printf("%s: Missing operand\nTry '%s --help' for more information.\n", argv[0], argv[0] );
        return 1;
    }

    double total_seconds = 0.0;

    for (int i = 1; i < argc; ++i) {
        double duration = parse_duration(argv[i]);
        if (duration < 0) {
            printf("Invalid time interval '%s'\nTry 'sleep --help' for more information.\n", argv[i]);
            return 1;
        }
        total_seconds += duration;
    }


    unsigned int whole = (unsigned int)total_seconds;
    unsigned int micro = (unsigned int)((total_seconds - whole) * 1e6);

    if (whole > 0) sleep(whole);
    if (micro > 0) usleep(micro);

    fflush (stdout);
    return 0;
}

__attribute__((constructor))
static void __sleep_register (void)
{
    builtin_command_register("sleep", sleep_main, sleep_pre, sleep_post);
}

