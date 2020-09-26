#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <termios.h>
#include "JobController.h"
#include "../shell.h"

void signalHandler(int signo);

pid_t shell_pgid;

void Init() {
    // Get shell pid
    shell_pgid = getpid();

    // Init signal handler
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    // signal(SIGCHLD, signalHandler);

    // Init processes list
    for (int i = 0; i < MAX_NO_JOBS; i++) {
        jobs[i].pid = -1;
        jobs[i].stat = Undefined;
        jobs[i].cmd[0] = 0;
    }
    foreground_job = -1;
}

void jobEnded(pid_t pid) {
    for (int i = 0; i < MAX_NO_JOBS; i++)
        if (jobs[i].pid == pid) {
            jobs[i].pid = -1;
            jobs[i].stat = Undefined;
            jobs[i].cmd[0] = 0;
            return;
        }
    PrintError("jobEnded", "Not found job");
}

void jobStopped(pid_t pid) {
    for (int i = 0; i < MAX_NO_JOBS; i++)
        if (jobs[i].pid == pid) {
            jobs[i].stat = Stopped;
            return;
        }
    PrintError("jobStopped", "Not found job");
}

void jobContinued(pid_t pid) {
    for (int i = 0; i < MAX_NO_JOBS; i++)
        if (jobs[i].pid == pid) {
            jobs[i].stat = Running;
            return;
        }
    PrintError("jobContinued", "Not found job");
}

void ZombieKiller() {
    int child_status;
    pid_t pid;
    // Update processes list
    for (int i = 0; i < MAX_NO_JOBS; i++)
        if (jobs[i].stat != Undefined) {
            // printf("[%d]%d\t%d\t\t%s\n", i+1, jobs[i].pid, jobs[i].stat, jobs[i].cmd);
            do {
                pid = waitpid(-jobs[i].pid, &child_status, WUNTRACED | WNOHANG);
                if (pid > 0) {
                    if (WIFEXITED(child_status) || WIFSIGNALED(child_status)) {
                        // jobEnded(jobs[i].pid);
                    } else if (WIFSTOPPED(child_status)) {
                        jobStopped(jobs[i].pid);
                        break;
                    } else if (WIFCONTINUED(child_status)) {
                        jobContinued(jobs[i].pid);
                        break;
                    }
                } else if (pid < 0) {
                    jobEnded(jobs[i].pid);
                }
            } while (pid > 0);
            // printf("%d %d %d %d\n", pid, jobs[i].pid, i, jobs[i].stat);
        }
}

void WaitForegroundProcess(bool cont) {
    if (foreground_job == -1) return;
    int stat;

    // Set foreground_pid process to be foreground process
    tcsetpgrp(STDIN_FILENO, foreground_job);
    if (cont)
        kill(foreground_job, SIGCONT);

    int pid;
    do {
        pid = waitpid(-foreground_job, &stat, WUNTRACED);

        if (WIFSTOPPED(stat)) {
            jobStopped(foreground_job);
            break;
        }
        printf("%d++\n", pid);
    } while (pid > 0);
    
    // Set shell_pgid process to be foreground process
    tcsetpgrp(STDIN_FILENO, shell_pgid);

    foreground_job = -1;
}

int findAvailableSlot() {
    for (int i = 0; i < MAX_NO_JOBS; i++)
        if (jobs[i].stat == Undefined)
            return i;
    return -1;
}

void startProcess(char** tokens, int infile, int outfile) {
    // Reset signal handle
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);

    // printf("%d %d\n", infile, outfile);
    if (infile != STDIN_FILENO) {
        dup2(infile, STDIN_FILENO);
        close(infile);
    }
    if (outfile != STDOUT_FILENO) {
        dup2(outfile, STDOUT_FILENO);
        close(outfile);
    }
    if (strcmp(tokens[0], "jobs") == 0) {
        ListJobs();
        Terminate();
    } else if (execvp(tokens[0], tokens) == -1) {
        char temp[100];
        sprintf(temp, "Run \"%s\" failed", tokens[0]);
        PrintError(temp, 0);
    }
    Terminate(1);
}

void StartJob(TokensHolder tokensHolder) {
    int jobSlot = findAvailableSlot();
    if (jobSlot == -1) {
        PrintError("Run failed", "Maximum number of jobs reached");
        return;
    }

    bool isBackgroundJob = (strcmp(tokensHolder->tokens[tokensHolder->count-1], "&") == 0);

    char* processTokens[MAX_INPUT_LENGTH];
    int processCount = 0;
    pid_t pgid = 0;
    int pip[2], infile = STDIN_FILENO, outfile = STDOUT_FILENO;
    for (int i = 0; i < tokensHolder->count+1; i++) 
        if (tokensHolder->tokens[i] == 0 || strcmp(tokensHolder->tokens[i], "|") == 0) {
            processTokens[processCount] = 0;
            processCount = 0;

            // Create pipe and set outfile
            if (i != tokensHolder->count) {
                if (pipe(pip) < 0) {
                    PrintError("pipe", 0);
                    Terminate();
                }
                outfile = pip[1];
            } else
                outfile = STDOUT_FILENO;

            // Fork to run process
            int pid = fork();
            if (pid < 0) {
                PrintError(tokensHolder->tokens[0], 0);
                Terminate();
            } else if (pid == 0) {
                if (pgid == 0) pgid = getpid();
                setpgid(getpid(), pgid);
                if (!isBackgroundJob)
                    tcsetpgrp(STDIN_FILENO, pgid);
                startProcess(processTokens, infile, outfile);
            } else {
                if (pgid == 0) pgid = pid;
                // In case of race condition
                setpgid(pid, pgid);
            }

            // Set infile
            if (i != tokensHolder->count)
                infile = pip[0];
        } else if (strcmp(tokensHolder->tokens[i], "&") == 0 && i == tokensHolder->count-1)
            continue;
        else {
            processTokens[processCount++] = tokensHolder->tokens[i];
        }
    
    // Add to processes list
    jobs[jobSlot].pid = pgid;
    jobs[jobSlot].stat = Running;
    strcpy(jobs[jobSlot].cmd, tokensHolder->tokens[0]);
    for (int i = 1; i < tokensHolder->count; i++)
        if (i != tokensHolder->count-1 || !isBackgroundJob) {
            strcat(jobs[jobSlot].cmd, " ");
            strcat(jobs[jobSlot].cmd, tokensHolder->tokens[i]);
        }
    

    if (!isBackgroundJob) {
        foreground_job = pgid;
        WaitForegroundProcess(false);
    } else {
        printf("Job ID: %d\n", jobSlot+1);
    }
}

const char* getStatusName(enum Status stat) {
    switch (stat) {
    case Stopped:
        return "Stopped";
        break;
    case Running:
        return "Running";
        break;
    }
}

void ListJobs() {
    for (int i = 0; i < MAX_NO_JOBS; i++)
        if (jobs[i].stat != Undefined) 
            printf("[%d]%d\t%s\t\t%s\n", i+1, jobs[i].pid, getStatusName(jobs[i].stat), jobs[i].cmd);
}

bool IsJobIDExist(pid_t jobID) {
    return (jobs[jobID-1].stat != Undefined);
}

void ContinueBackground(pid_t jobID) {
    kill(-jobID, SIGCONT);
}

void ContinueForeground(pid_t jobID) {
    foreground_job = jobs[jobID-1].pid;
    WaitForegroundProcess(true);
}

void KillAllJob() {
    for (int i = 0; i < MAX_NO_JOBS; i++)
        if (jobs[i].stat != Undefined)
            kill(-jobs[i].pid, SIGKILL);
}