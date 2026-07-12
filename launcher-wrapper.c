#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>


int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr,
                "usage: wrapper <pidfile> <program> [args...]\n");
        return 1;
    }

    const char *pidfile = argv[1];
    printf("opening %s\n", pidfile);
    FILE *f = fopen(pidfile, "w+");

    if (!f) {
        perror("fopen");
        return 1;
    }

    fprintf(f, "%d\n", getpid());
    fclose(f);

    int pid = fork();
    if(!pid){
        execvp(argv[2], &argv[2]);
        exit(0);
    }
    int status;
    waitpid(pid,&status,0);
    remove(pidfile);

    perror("execvp");
    return 1;
}