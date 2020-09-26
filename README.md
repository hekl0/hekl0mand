# Hekl0mand

Project 1 for CSC 256. A simple controlling terminal implemented in C with custom UI and personalized.

### Supported command:
- **help**: The help command will print out help message
- **exit**: The exit command will terminate the terminal
- **cd**: The cd command will change the working directory of the shell process
- **jobs**: The jobs command will print the jobs in the background
- **bg**: The bg command will move a job into the background. Its format is `bg %id` where id is replaced by the job ID reported by the jobs command
- **fg**: The fg command will moves a job into the foreground. Its format is `fg %id` where id is replaced by the job ID reported by the jobs command

To run outside programs (the program will be run in background if *&* is included):
- `<program1> <arglist1> | <program2> <arglist2> | ... | <programN> <arglistN> [&]`

### Running instructions 

- Compile: `make shell`
- Run: `./shell`
