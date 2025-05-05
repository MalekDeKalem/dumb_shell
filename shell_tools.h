#ifndef _SHELL_TOOLS_H_
#define _SHELL_TOOLS_H_

#include <stdio.h>
#include <unistd.h>

#define TOKEN_DELIM " \t\n\r"
#define BUFSIZE 1024
#define TOKENSIZE 64
#define OPSIZE 16

typedef enum {
  CD,
  HELP,
  EXIT,
  PIPE,
  SIZE,
  NONE
} Ops;

int pipeHandler(char** args)
{
  int pipefd[2];
  pipe(pipefd);

  pid_t pid2 = fork();

}


int dshCd(char** args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "Failed to change directory\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("dsh");
    }
  }

  return 1;
}


int dshHelp(char** args)
{
  printf("Dumb Shell by MalekDeKalem\n");
  printf("This Shell is still a work in progress\n");
  return 1;
}

int dshExit(char** args)
{
    return 0;
}



#endif // _SHELL_TOOLS_H_ 
