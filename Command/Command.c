#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include "Command.h"
#include "../JobController/JobController.h"

void exitcmd();
void cd(char *dest);
void pwd();
void bg(TokensHolder tokensHolder);
void fg(TokensHolder tokensHolder);
void run_program(TokensHolder tokensHolder);

void ProcessCommand(TokensHolder tokensHolder) {
    if (tokensHolder->count == 0) return;
    if (tokensHolder->count > 90) {
        PrintError("Error", "Too many args, maybe try fewer?");
        return;
    }
    if (strcmp(tokensHolder->tokens[0], "help") == 0) {
        // TODO: Making help function :((
        PrintOutput(
"hekl0mand (made in 2020)\n"
"\n"
"Supported command:\n"
"\t help: The help command will print out help message\n"
"\t exit: The exit command will terminate the terminal\n"
"\t cd: The cd command will change the working directory of the shell process\n"
"\t jobs: The jobs command will print the jobs in the background\n"
"\t bg: The bg command will move a job into the background. Its format is `bg %id` where id is replaced by the job ID reported by the jobs command\n"
"\t fg: The fg command will moves a job into the foreground. Its format is `fg %id` where id is replaced by the job ID reported by the jobs command\n"
"\n"
"To run outside programs (the program will be run in background if & is included):\n"
"\t <program1> <arglist1> | <program2> <arglist2> | ... | <programN> <arglistN> [&]\n"
        );
        return;
    } else if (strcmp(tokensHolder->tokens[0], "exit") == 0) {
        if (tokensHolder->count != 1) {
            PrintError("exit", "too many arguements");
            return;
        }
        exitcmd();
    } else if (strcmp(tokensHolder->tokens[0], "cd") == 0) {
        if (tokensHolder->count != 2) {
            PrintError("cd", "too many arguements");
            return;
        }
        cd(tokensHolder->tokens[1]);
    } else if (strcmp(tokensHolder->tokens[0], "pwd") == 0) {
        if (tokensHolder->count != 1) {
            PrintError("pwd", "too many arguements");
            return;
        }
        pwd();
    } else if (strcmp(tokensHolder->tokens[0], "bg") == 0) {
        if (tokensHolder->count != 2) {
            PrintError("bg", "too many arguements");
            return;
        }
        bg(tokensHolder);
    } else if (strcmp(tokensHolder->tokens[0], "fg") == 0) {
        if (tokensHolder->count != 2) {
            PrintError("fg", "too many arguements");
            return;
        }
        fg(tokensHolder);
    } else {
        run_program(tokensHolder);
    }
}

void exitcmd() {
    Terminate();
}

void cd(char* dest) {
    if (chdir(dest) != 0) {
        PrintError("cd Failed", 0);
    }
}

void pwd() {
    char path[100];
	getcwd(path, 100);
    PrintOutput(path);
}

int strToInt(char* str) {
    if (strlen(str) > 9) return -1;
    for (int i = 0; i < strlen(str); i++)
        if (str[i] < '0' || str[i] > '9')
            return -1;
    return strtol(str, NULL, 0);
}

void bg(TokensHolder tokensHolder) {
    pid_t jobID = strToInt(tokensHolder->tokens[1]);
    if (jobID == -1 || !IsJobIDExist(jobID)) {
        PrintError("bg", "No such job");
        return;
    }
    ContinueBackground(jobID);
}

void fg(TokensHolder tokensHolder) {
    pid_t jobID = strToInt(tokensHolder->tokens[1]);
    if (jobID == -1 || !IsJobIDExist(jobID)) {
        PrintError("fg", "No such job");
        return;
    }
    ContinueForeground(jobID);
}

void run_program(TokensHolder tokensHolder) {
    StartJob(tokensHolder);
}