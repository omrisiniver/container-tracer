#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    sleep(3);
    int counter = 0;
    while (counter < 40)
    {
        sleep(1);
        printf("test2 %d\n", counter++);
    }
}
