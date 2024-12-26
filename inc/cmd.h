#ifndef MYSHELL_CMD_H
#define MYSHELL_CMD_H

typedef struct cmd {
  char **args;
  int input_fd;
  int output_fd;
  int background;
  struct cmd *next;
} cmd_t;

cmd_t *parse_input(char *input);

void exec_cmd(cmd_t *cmd);
void exec_single_cmd(cmd_t *cmd);
void exec_pipeline(cmd_t *cmd);

#endif // MYSHELL_CMD_H
