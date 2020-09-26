#ifndef _CustomIO_h
#define _CustomIO_h

#include "../shell.h"

#define MAX_INPUT_LENGTH 100

typedef struct TokensHolder * TokensHolder;
struct TokensHolder {
    int count;
    char* tokens[MAX_INPUT_LENGTH];
};

// PrintHeader print the header of the terminal line
// Args:
//  - username: system username
//  - path: current directory
void PrintHeader(char* username, char* path);
// ReadCommand read the next command from user
// Args:
//  - username: system username
//  - path: current directory
// Return:
//  - TokensHolder contains tokenized command inputted by user
TokensHolder ReadCommand(char* username, char* path);
// PrintOutput print output to stdout
// Args:
//  - output: the string to be writtent
void PrintOutput(char *output);
// PrintError print error to stderr in format hekl0mand: cmd: desc
// Args:
//  - cmd: command that run failed
//  - desc: description of the error, if desc is NULL, desc will be set as errno
void PrintError(char *cmd, char* desc);

#endif