#ifndef __YAR_JOB_H
#define __YAR_JOB_H

#include <sys/types.h>
#include <termios.h>

#include "data_structure/string.h"

typedef struct process
{
  struct process *next;
  string *environ;
  int argc;
  string *argv;
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
  int foreground;
} job;

process *new_process ();
void free_process (process *);

job *new_job ();
void free_job (job *);

void format_job_info (job *j, const char *status);

void wait_for_job (job *j);

void put_job_in_foreground (job *j, int cont);
void put_job_in_background (job *j, int cont);
void continue_job (job *j);

job * find_job (pid_t pgid);
int job_is_stopped (job *j);
int job_is_completed (job *j);

void do_job_notification (void);
int mark_process_status (pid_t pid, int status);

extern job *first_job;


#endif
