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
    printf("this is the PID: %d \n", pid );
    /* First Step: Create the first process to run the program from the command line */
    execl(program, program, NULL);

    pid_t parentPID = getppid();

    printf("this is parent PID: %d \n", parentPID);
    /* Second step: Create the pipe to be used for connecting procmon to filter */
    pid_t child = fork();
    printf("childPID: %d \n", child);
    if (child == -1){
      perror("fork");
      exit(1);
    }else if(child == 0){
      printf("this is the childPID: %d \n", child);
    }else{
      //execl("filter", "filter", NULL);
      printf("PID after filter execution %d \n", getpid());
    }

    /* Third step: Lets create the filter process - don't forget to connect to the pipe */

    /* Fourth step: Lets create the procmon process - don't forget to connect to the pipe */
    //execl("procmon", "procmon", NULL);
    printf("running procmon \n");
    /* Fifth step: Let things run for 20 seconds */
    sleep(20);
    /* Last step:
       1. Kill the process running the program
       2. Sleep for 2 seconds
       3. Kill the procmon and filter processes
    */


    return(0);  /* All done */
}
