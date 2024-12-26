
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "job.h"

Job *job_list = NULL;

void add_job(pid_t pid, char *cmd) {
  Job *new_job = malloc(sizeof(Job));
  new_job->pid = pid;
  new_job->cmd = strdup(cmd);
  new_job->status = RUNNING;
  new_job->next = job_list; // add front of the list
  job_list = new_job;
}
Job *get_current_job() {
  // comment
  return job_list;
}
