#ifndef __YAR_JOB_H
#define __YAR_JOB_H

#include <sys/types.h>
#include <termios.h>

#include "data_structure/string.h"

typedef struct process
{

  struct process *next;
  string *argv;
  char **env;
  pid_t pid;
  char completed;
  char stopped;
  int status;
  int stdin, stdout, stderr;

} process;

typedef struct job
{
  struct job *next;
  string command;
  process *first_process;
  pid_t pgid;
  char notified;
  struct termios tmodes;
  int stdin, stdout, stderr;
} job;

void free_process (process *);
void free_job (job *);

void format_job_info (job *j, const char *status);

void wait_for_job (job *j);

void put_job_in_foreground (job *j, int cont);

void put_job_in_background (job *j, int cont);

#endif
