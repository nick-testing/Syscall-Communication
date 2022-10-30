#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define LINE_SIZE 100

#define READ_LINE(line, f) { \
                    fgets(line, LINE_SIZE, f); \
                    line[strlen(line) - 1] = '\0'; \
                    }

// SIGALRM handler
void sigalrmHandler(int sig) {
    fprintf(stdout, "The server was closed because no service request was received for the last 60 seconds\n");
    exit(EXIT_SUCCESS);
}


void usrRequestHandler(int sig) {
    pid_t child;
    if ((child = fork()) == 0) {
        // Read and close 'to_srv' file
        FILE *f = fopen("to_srv", "r");
        char line[LINE_SIZE];
        char pidStr[LINE_SIZE];
        READ_LINE(line, f);
        strcpy(pidStr, line);

        int pid = atoi(line);
        READ_LINE(line, f);
        int num1 = atoi(line);
        READ_LINE(line, f);
        int operator = atoi(line);
        READ_LINE(line, f);
        int num2 = atoi(line);

        fclose(f);
        remove("to_srv");


        int result = 0;
        switch(operator) {
            case 1:
                result = num1 + num2;
                break;
            case 2:
                result = num1 - num2;
                break;
            case 3:
                result = num1 * num2;
                break;
            case 4:
                // Send SIGFPE if attempting to divide by 0
                if (num2 == 0) {
                    kill(pid, SIGFPE);
                    exit(-1);
                }
                result = num1 / num2;
                break;
            default:
                exit(-1);
        }

        // Write result of applied function to a shared file
        char resultFName[strlen("to_client_") + LINE_SIZE + 1];
        strcpy(resultFName, "to_client_");
        strcat(resultFName, pidStr);
        FILE *resultF = fopen(resultFName, "w");
        fprintf(resultF,"%d", result);
        fclose(resultF);
        kill(pid, SIGUSR1);
        exit(EXIT_SUCCESS);
    }
    signal(SIGUSR1, usrRequestHandler);
}



int main(void) {
    // Remove 'to_srv' if exists
    if (access("to_srv", F_OK) == 0)
        remove("to_srv");

    // Initialize signal handlers
    signal(SIGALRM, sigalrmHandler);
    signal(SIGUSR1, usrRequestHandler);

    while (true) {
        alarm(60);
        pause();
        alarm(NULL);
    }
}
