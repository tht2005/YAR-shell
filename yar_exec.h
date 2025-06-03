#ifndef __YAR_EXEC_H
#define __YAR_EXEC_H

#include "yar_ast.h"
#include "yar_job.h"

void launch_process (process *p, pid_t pgid, int foreground);
void launch_job (job *j);
process *__ast_build_process (command *command);

#endif
