#include "builtin_commands.h"

#include <stdio.h>
#include <string.h>
#include "../yar_job.h"

void jobs_pre ()
{
    // khoi tao?
}

void jobs_post ()
{
    // giai phong bo nho?
}

int jobs_main(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        puts ("List all running job status.");
        return 0;
    } 
    for (job *j = first_job; j; j = j->next)
    {
        format_job_info (j, job_is_stopped (j)
                                    ? "Stopped"
                                    : (job_is_completed(j) ? "Completed" : "Running") );
    }
    return 0;
}

__attribute__((constructor))
static void __jobs_register (void)
{
    builtin_command_register("jobs", jobs_main, jobs_pre, jobs_post);
}

