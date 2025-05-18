#include "shell_tools.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

char* readLine(void)
{
  int bufferSize = BUFSIZE;
  char* buffer = (char*)malloc(sizeof(char) * bufferSize);
  int pos = 0;
  int character;

  if (!buffer) {
    fprintf(stderr, "Failed to allocate buffer\n");
    exit(EXIT_FAILURE);
  }

  while (1) {

    // Get the current character 
    character = getchar();

    if (pos >= bufferSize) {
      bufferSize += BUFSIZE;
      buffer = (char*)realloc(buffer, bufferSize);
      if (!buffer) {
        fprintf(stderr, "Failed to reallocate the buffer\n");
      }
    }
    
    if (character == EOF || character == '\n') {
      // null terminate the line
      buffer[pos] = '\0';
      return buffer;
    } else {
      buffer[pos] = character;
    }

    pos++;
  }

}

char** splitLine(char* line)
{
  int bufferSize = TOKENSIZE;
  char* token;
  char** tokens = (char**)malloc(bufferSize * sizeof(char*));
  int pos = 0;

  if (!tokens) {
    fprintf(stderr, "failed to allocate tokens\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, TOKEN_DELIM);
  while (token != NULL) {
    tokens[pos] = token;
    pos++;

    if (pos >= bufferSize) {
      bufferSize *= 2;
      tokens = (char**)realloc(tokens, bufferSize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "Failed to reallocate tokens\n");
        exit(EXIT_FAILURE);
      }

    }

      token = strtok(NULL, TOKEN_DELIM);
  }

  tokens[pos] = NULL; 
  return tokens;
}

Ops* parseTokens(char** tokens) 
{
  int bufferSize = OPSIZE;
  Ops* operations = (Ops*)malloc(bufferSize * sizeof(Ops));
  int ptr = 0;
  int i;

  for (i = 0; tokens[i] != NULL; i++) {

    if (ptr >= bufferSize) {
      bufferSize *= 2;
      operations = (Ops*)realloc(operations, bufferSize * sizeof(Ops));
    }

    if (strcmp(tokens[i], "cd") == 0) {
      operations[ptr] = CD;
    } else if (strcmp(tokens[i], "exit") == 0) {
      operations[ptr] = EXIT;
    } else if (strcmp(tokens[i], "help") == 0) {
      operations[ptr] = HELP;
    } else if (strcmp(tokens[i], "|") == 0) {
      operations[ptr] = PIPE;
    } else if (strcmp(tokens[i], "echo") == 0) {
      operations[ptr] = ECHO;
    }

    ptr++;
  }

  operations[ptr] = NONE;

  return operations;
}

int dshLaunch(char** args)
{
  pid_t pid;
  pid_t wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("dsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("dsh");
  } else {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int dshExecute(Ops* operations, char** args)
{
  int i;

  if (args[0] == NULL) return 1;

  for (i = 0; operations[i] != NONE; i++) {
    switch (operations[i]) {
      case CD:
        return dshCd(args);
      case HELP:
        return dshHelp(args);
      case EXIT:
        return dshExit(args);
      case ECHO:
        return dshEcho(args);
      default:
        break;

    }
  }

  return dshLaunch(args); 
}



void dshLoop()
{
  char* line; 
  char** args;
  Ops* operations;
  int status;

  do
  {
    printf("dsh> ");
    line = readLine();
    args = splitLine(line);
    operations = parseTokens(args); 
    status = dshExecute(operations, args);


    free(line);
    free(args);
    free(operations);
  } while (status);


}


int main()
{
  dshLoop();
  return EXIT_SUCCESS;
}


