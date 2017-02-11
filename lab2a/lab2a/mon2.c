/*--------------------------------------------------------------------------
File: mon2.c

Description: This program creates a process to run the program identified
             on the commande line.  It will then start procmon in another
	     process to monitor the change in the state of the first process.
	     After 20 seconds, signals are sent to the processes to terminate
	     them.

	     Also a third process is created to run the program filter.
	     A pipe is created between the procmon process and the filter
	     process so that the standard output from procmon is sent to
	     the standard input of the filter process.
--------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>


/* It's all done in main */
int main(int argc, char **argv)
{
    char    *program;

    if (argc != 2) {
        printf("Usage: mon2 fileName\n where fileName is an executable file.\n");
        exit(-1);
    } else
        program = argv[1];  /* This is the program to run and monitor */

    pid_t pid = getpid();
    printf("\n");
    printf("mon2 PID: %d \n", pid );
    /* First Step: Create the first process to run the program from the command line */
    pid_t programPID = fork();
    if (programPID <0){
      perror("program failed");
    }else if(programPID == 0){
      printf("running program \n");
      if (execl(program, program, NULL) == -1){
        perror("program failed");
      }
    }

    /* Second step: Create the pipe to be used for connecting procmon to filter */
    int fd[2];
    int READ = 0;
    int WRITE = 1;
    pipe(fd);


    pid_t filterPID = fork();
    if (filterPID == -1){
      perror("fork");
    }else if(filterPID == 0){
      /* Third step: Lets create the filter process - don't forget to connect to the pipe */
      printf("running filter \n");
      close(fd[WRITE]);
      dup2(fd[READ],READ);
      if (execl("filter", "filter", NULL) == -1){
        perror("filter failed");
      }
      close(fd[READ]);
    }
      /* Fourth step: Lets create the procmon process - don't forget to connect to the pipe */

    printf("running procmon \n");

    dup2(fd[WRITE], WRITE);

    char programPIDString[9];
    snprintf(programPIDString,10,"%d", programPID);
    pid_t procmonPID = fork();
    if (procmonPID == 0){
      if (procmonPID < 0){
          perror("procmon fork()");
      }
      else if (execl("procmon", "procmon", programPIDString, NULL) == -1){
      perror("procmon failed");
      }
    }
    close(fd[WRITE]);


    printf("program PID: %d \n", programPID);
    printf("filter PID: %d \n", filterPID);
    printf("procmon PID: %d \n", procmonPID );

    /* Fifth step: Let things run for 20 seconds */
    sleep(20);
    /* Last step:
       1. Kill the process running the program
       2. Sleep for 2 seconds
       3. Kill the procmon and filter processes
    */
    kill(programPID, SIGKILL);
    sleep(2);
    kill(filterPID, SIGKILL);
    kill(procmonPID, SIGKILL);

    return(0);  /* All done */
}
