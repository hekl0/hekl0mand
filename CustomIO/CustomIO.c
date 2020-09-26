#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "CustomIO.h"

size_t lenInput = MAX_INPUT_LENGTH; 
char input[MAX_INPUT_LENGTH];
char delim[] = " \t";

char* UsernameColor = ANSI_COLOR_GREEN;
char* PathColor = ANSI_COLOR_BLUE;
char* ColorReset = ANSI_COLOR_RESET;

extern int errno;

void scanning(char *input);
void tokenizing(char* str, char* delim, TokensHolder tokensHolder);

void PrintHeader(char* username, char* path) {
    printf("%s%s%s:%s%s%s$ ", UsernameColor, username, ColorReset, PathColor, path, ColorReset);
    fflush(stdout);
}

TokensHolder newTokensHolder() {
    TokensHolder tokensHolder = malloc(sizeof(TokensHolder));
    tokensHolder->count = 0;
    for (int i = 0; i < MAX_INPUT_LENGTH; i++)
        tokensHolder->tokens[i] = 0;
    return tokensHolder;
}

TokensHolder ReadCommand(char* username, char* path) {
    PrintHeader(username, path);
    scanning(input);
    TokensHolder tokensHolder = newTokensHolder();
    tokenizing(input, delim, tokensHolder);
    return tokensHolder;
}

void PrintOutput(char* output) {
    printf("%s\n", output);
    fflush(stdout);
}

void PrintError(char *cmd, char* desc) {
    if (desc == 0)
        desc = strerror(errno);
    printf(" -%s: %s: %s\n", Interpreter, cmd, desc);
    fflush(stdout);
}

void scanning(char* input) {
    getline(&input, &lenInput, stdin);
    if (feof(stdin)) { // EOF is inputted
        printf("\n");
        Terminate();
    }
    if (strlen(input) > 0)
        input[strlen(input)-1] = 0; // Remove end of line character
}

void tokenizing(char* str, char* delim, TokensHolder tokensHolder) {
	char *ptr = strtok(str, delim);
	int i = 0;
	while (ptr != NULL) {
		tokensHolder->tokens[i] = ptr;
		ptr = strtok(NULL, delim);
		i++;
	}
    tokensHolder->count = i;
}