#ifndef _ProcessController_h
#define _ProcessController_h

#include <stdbool.h>
#include "../CustomIO/CustomIO.h"

#define MAX_NO_JOBS 100

enum Status{Undefined, Running, Stopped};

struct Job {
    pid_t pid;
    enum Status stat;
    char cmd[MAX_INPUT_LENGTH];
};
struct Job jobs[MAX_NO_JOBS];

// PID of the proground process, -1 if there is no foreground process
int foreground_job;

// Init initialize process controller and redefine signal handler
void Init();
// StartProcess starts a process based on the command inputted
void StartJob(TokensHolder tokensHolder);
// WaitForegroundProcess block the current process and wait for the foreground_job to finish
void WaitForegroundProcess(bool cont);
// ListJobs will list all jobs in the background or is stopped
void ListJobs();
// ContinueBackground continue job with jobID in background
void ContinueBackground(pid_t jobID);
// ContinueForeground continue job with jobID in foreground
void ContinueForeground(pid_t jobID);
// Check if jobID exist in jobs list
bool IsJobIDExist(pid_t jobID);
// Kill all jobs in background
void KillAllJob();
// Update status of background processes
void ZombieKiller();

#endif