#ifndef __YAR_EXEC_H
#define __YAR_EXEC_H

#include "yar_job.h"

#include "data_structure/string.h"

void launch_process (process *p, pid_t pgid, int foreground);
void launch_job (job *j, int foreground);

#endif
