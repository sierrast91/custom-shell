#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cmd.h"
#include "job.h"

#define MAX_ARGS 100
#define MAX_COMMAND_LENGTH 4096

char *username;
char hostname[256];
char cwd[1024];

void prompt(const char *info) {
  if (info == NULL) {
    printf("\033[1;32m%s@%s\033[0m:\033[1;33m%s\033[0m$ ", username, hostname,
           cwd);
  } else {
    printf("\n%s\n\033[1;32m%s@%s\033[0m:\033[1;33m%s\033[0m$ ", info, username,
           hostname, cwd);
  }
  fflush(stdout);
}

void handle_fork(char **args) {
  pid_t pid = fork(); // create new process;
  if (pid == -1) {
    perror("failed to fork");
  } else if (pid == 0) {
    // child-process : execute the command
    if (execvp(args[0], args) == -1) {
      perror("execution failed");
    }
    exit(1);
  } else {
    // parent-process : wait for execution
    int status;
    add_job(pid, args[0]);
    waitpid(pid, &status, 0);
  }
}

void execute_command(char **args) {
  // exit
  if (args[0] != NULL && strcmp(args[0], "exit") == 0) {
    exit(0);
  }
  // redirect
  int redirect_index = -1;
  for (int i = 0; args[i] != NULL; i++) {
    if (strcmp(args[i], ">") == 0) {
      redirect_index = i;
    }
  }
  if (redirect_index != -1) {
    args[redirect_index] = NULL;
    const char *filename = args[redirect_index + 1];

    int saved_stdout = dup(STDOUT_FILENO);
    if (saved_stdout == -1) {
      perror("dup");
      return;
    }

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
      perror("failed to open file");
      return;
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);

    handle_fork(args);

    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
    return;
  }
  // cd command
  if (args[0] != NULL && strcmp(args[0], "cd") == 0) {
    char *dir = args[1] ? args[1] : getenv("HOME");
    if (chdir(dir) != 0) {
      perror("cd failed");
    }
    getcwd(cwd, sizeof(cwd));
    return;
  }
  // handling external commands
  handle_fork(args);
}
int main() {

  char input[MAX_COMMAND_LENGTH];
  char *args[MAX_ARGS];

  // personalize shell
  username = getenv("USER");
  gethostname(hostname, sizeof(hostname));
  getcwd(cwd, sizeof(cwd));
  // run shell
  while (1) {
    prompt(NULL);
    if (fgets(input, MAX_COMMAND_LENGTH, stdin) == NULL) {
      break; // exit on EOF (ctrl+d)
    }
    parse_input(input);

    // execute_command(args);
  }
  // run command here
  return 0;
}
