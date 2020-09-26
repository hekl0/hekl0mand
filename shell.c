#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include "CustomIO/CustomIO.h"
#include "Command/Command.h"
#include "JobController/JobController.h"

TokensHolder th;

void freeTokensHolder() {
	if (th != NULL)
		free(th);
}

void Terminate() {
	KillAllJob(); // No orphan process!!!
	freeTokensHolder(); // No memory leak!!!
	exit(0);
}

// GetUsername return the system username if found, else return "anon"
char* getUsername() {
	register struct passwd* pw;
	pw = getpwuid(geteuid());
	if (pw) 
		return pw->pw_name;
    else 
		return "anon";
}

int main (int argc, char ** argv) {
	dup2(STDOUT_FILENO, STDERR_FILENO); // Redirect stderr to stdout
	Init();
	while (1) {
        usleep(10000); // Wait for the writer to finish
		ZombieKiller();
		getcwd(path, 100);
		username = getUsername();
		th = ReadCommand(username, path);
		ProcessCommand(th);
		freeTokensHolder();
		fflush(stdout);
	}
	return 0;
}
