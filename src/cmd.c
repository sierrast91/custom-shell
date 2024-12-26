#include "cmd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void free_args(int arglen, char **args) {
  for (int i = 0; i < arglen; i++) {
    free(args[i]);
  }
  free(args);
}

void append_token(int *arglen, char ***args, char *token) {
  char **nargs = realloc(*args, sizeof(char *) * (*arglen + 1));
  if (nargs == NULL) {
    perror("realloc");
    return;
  }
  *args = nargs;                    // increase length
  (*args)[*arglen] = strdup(token); // copy token
  (*arglen)++;
  return;
}
cmd_t *parse_input(char *input) {
  cmd_t *cmd = malloc(sizeof(cmd_t));
  // tokenize
  int arg_count = 0;
  char **args = NULL;
  char *token = strtok(input, " \n");
  while (token != NULL) {
    append_token(&arg_count, &args, token);
    token = strtok(NULL, " \n");
  }
  // print
  for (int i = 0; i < arg_count; i++) {
    printf("%s\n", args[i]);
  }
  return cmd;
}
void exec_cmd(cmd_t *cmd) {
  if (cmd->next == NULL) {
    exec_single_cmd(cmd);
  } else {
    exec_pipeline(cmd);
  }
}
void exec_single_cmd(cmd_t *cmd) {
  if (cmd->background) {
    // background process
    pid_t pid = fork();
    if (pid == 0) {
      // child process
      if (cmd->input_fd != STDIN_FILENO) {
        dup2(cmd->input_fd, STDIN_FILENO);
        close(cmd->input_fd);
      }
      if (cmd->output_fd != STDOUT_FILENO) {
        dup2(cmd->output_fd, STDOUT_FILENO);
        close(cmd->output_fd);
      }
      execvp(cmd->args[0], cmd->args); // execute command
    } else {
      // parent process dont't wait for the child if running in the backgroudn
      printf("[started background process with PID %d]\n", pid);
    }
  } else {
    // foreground process
    if (fork() == 0) {
      // child process
      execvp(cmd->args[0], cmd->args); // exec cmd
    } else {
      // parent process
      wait(NULL);
    }
  }
}
void exec_pipeline(cmd_t *cmd) {
  int pipe_fd[2];
  pid_t pid;
  int input_fd = STDIN_FILENO;
  while (cmd != NULL) {
    pipe(pipe_fd); // create a pipe

    if ((pid = fork()) == 0) {
      // child process
      if (input_fd != STDIN_FILENO) {
        dup2(input_fd, STDIN_FILENO); // read from input
        close(input_fd);
      }

      if (cmd->next != NULL) {
        dup2(pipe_fd[1], STDOUT_FILENO);
      }
      close(pipe_fd[0]);
      execvp(cmd->args[0], cmd->args);
    } else {
      // parent process
      wait(NULL);
      close(pipe_fd[1]);
      input_fd = pipe_fd[0]; // next command reads from this pipe
      cmd = cmd->next;       // move to next cmd
    }
  }
}
