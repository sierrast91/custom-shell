#ifndef MYSHELL_JOB_H
#define MYSHELL_JOB_H

#include <sys/wait.h>

typedef struct Job {
  pid_t pid;
  char *cmd;
  int status;
  struct Job *next;
} Job;

typedef enum {
  RUNNING = 0,
  STOPPED,
} JobState;

void add_job(pid_t pid, char *cmd);

Job *get_current_job();

extern Job *job_list;
#endif // MYSHELL_JOB_H
