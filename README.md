# Hekl0mand

Project 1 for CSC 256. A simple controlling terminal implemented in C with custom UI and personalized.

### Supported command:
- **exit**: The exit command should use the exit() system call to terminate
the shell.
- **cd**: The cd command uses the chdir() system call to change the working
directory of the shell process.
- **jobs**: The jobs command should print the jobs in the background and
should print a) an index number for the command line that was run in the
background and b) the command line that was placed into the background.
The format should be: `<id>: <program name> <arg1> <arg2> .... <argN>`
Note that each set of processes on a command line comprise a job.
- **bg**: The bg command moves a job into the background. Its format is `bg
%id` where id is replaced by the job ID reported by the jobs command.
- **fg**: The fg command moves a job into the foreground. Its format is `fg
%id` where id is replaced by the job ID reported by the jobs command.

To run outside programs (the program will be run in background if *&* is included):
- `<program1> <arglist1> | <program2> <arglist2> | ... | <programN> <arglistN> [&]`

### Running instructions 

- Compile: `make shell`
- Run: `./shell`
