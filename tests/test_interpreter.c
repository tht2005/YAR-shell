#include "yar_interpreter.h"

void test (char *input)
{
    fprintf (stderr, "input=`%s`\n", input);
    interpret (input);
}

int main ()
{
    test ("\"  ${USER}  $USER @@ @ @ @   \"abxemrovzvz\'cxjdf  jdk   djk  @@@ \'");
    test ("\"  ${USER}  $USER @@ @ @ @   \"abxemrovzvz\'cxjdf  jdk   djk  @@@ \'\" j kd  jdk jk  \"");

    // must failed gracefully not cause error
    test ("\"  ${USER}  $USER @@ @ @ @   \"abxemrovzvz\'cxjdf  jdk   djk  @@@ \'\" j kd  jdk jk  ");
    test ("\"  ${USER}  $USER @@ @ @ @   \"abxemrovzvz\'cxjdf  jdk   djk  @@@ \'\" j kd  jdk jk  \'");
    return 0;
}

