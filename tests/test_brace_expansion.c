#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "libglob/glob.h"

void test (const char *pattern)
{
    glob_t results;
    int ret = g_glob (pattern, GLOB_BRACE | GLOB_NOCHECK, NULL, &results);
    printf ("pattern=`%s`\n", pattern);
    if (ret == 0) {
        for (size_t i = 0; i < results.gl_pathc; ++i) {
            puts (results.gl_pathv[i]);
        }
    }
    else if (ret == GLOB_NOMATCH)
    {
        puts ("No match");
    }
    else {
        printf ("error (code=%d)\n", ret);
    }
    g_globfree (&results);
}

int main()
{
    // test ("{1,2,3}");
    // test ("{1,2,abc{1,2}}");
    // test ("{1,2,abc\\{1,2\\}}");
    // test ("\\{1,2,3\\}");
    // test ("*");
    test ("{{1,5}{a,b}}");
    test ("*");
    test ("\\{a,b\\}{c,d,e}");
    test ("{*.txt,*.tmp}");
    test ("\\a\\b\\d\\{\\}\\}\\{");
    return 0;
}
