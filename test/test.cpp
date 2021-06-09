#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    sleep(3);
    int child_pid = fork();
    if(child_pid == 0) {
        /* This is done by the child process. */

        execv("test2", argv);
    
        /* If execv returns, it must have failed. */

        printf("Unknown command\n");
    }
  else 
  {
    sleep(3);
    int counter = 0;
    while (counter < 40)
    {
        sleep(1);
        printf("check %d\n", counter++);
    }
  }
}
