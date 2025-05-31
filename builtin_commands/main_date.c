#include <dirent.h>
#include <errno.h>

#include "builtin_commands.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../data_structure/string.h"

void date_pre ()
{
}

void date_post ()
{
}

int date_main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: date [FORMAT]\n\n");
        printf("Display the current date and time in the specified format.\n");
        printf("If no format is given, the default format is used.\n\n");

        printf("Format directives:\n");
        printf("  %%Y    Year (e.g., 2025)\n");
        printf("  %%m    Month (01–12)\n");
        printf("  %%d    Day of the month (01–31)\n");
        printf("  %%H    Hour (00–23)\n");
        printf("  %%M    Minute (00–59)\n");
        printf("  %%S    Second (00–59)\n");
        printf("  %%a    Abbreviated weekday name (e.g., Sun)\n");
        printf("  %%A    Full weekday name (e.g., Sunday)\n");
        printf("  %%c    Locale’s date and time representation\n\n");

        fflush(stdout);
        return 0;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char *format, buffer[255];

    if (argc == 1) {
        format = "%c";
    }
    else {
        format = argv[1];
    }

    if (strftime(buffer, sizeof(buffer), format, t)) {
        printf("%s\n", buffer);
    } else {
        printf("Error: Format string is too long or invalid.\n");
        return 1;
    }

    fflush (stdout);
    return 0;
}

__attribute__((constructor))
static void __date_register (void)
{
    builtin_command_register("date", date_main, date_pre, date_post);
}

