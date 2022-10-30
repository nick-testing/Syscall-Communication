#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define LINE_SIZE 100

// returns a random number from {0, 5}
int getrandom() {
    srand(time(NULL));
    return rand() % 6;
}

// SIGALRM handler
void sigalrmHandler(int sig) {
    fprintf(stdout, "Client closed because no response was received from the server for 30 seconds\n");
    exit(-1);
}

// SIGUSR1 handler
void sigusrHandler(int sig) {
    pid_t pid = getpid();
    char pidStr[10];
    char line[LINE_SIZE];
    sprintf(pidStr, "%d", pid);
    char fileName[LINE_SIZE + strlen("to_client_") + 1];
    strcpy(fileName, "to_client_");
    strcat(fileName, pidStr);

    FILE *f = fopen(fileName, "r");
    fgets(line, LINE_SIZE, f);
    fclose(f);
    remove(fileName);
    fprintf(stdout, "%s\n", line);

}

//SIGFPE handler
void zeroDivHandler(int sig) {
    printf("CANNOT_DIVIDE_BY_ZERO\n");
    exit(-1);
}


int main(int argc, char* argv[]) {
    // Initialize signal handlers
    signal(SIGALRM, sigalrmHandler);
    signal(SIGUSR1, sigusrHandler);
    signal(SIGFPE, zeroDivHandler);

    // Input check
    if (argc < 5) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    int serverID, firstNumber, function, secondNumber;

    // Check input - if fourth and second arguments 0 and not matching the actual argument - exit
    if (!(serverID = atoi(argv[1])) || !(function = atoi(argv[3])) ||
        (!(firstNumber = atoi(argv[2])) && strcmp("0", argv[2]) != 0) ||
        (!(secondNumber = atoi(argv[4])) && strcmp("0", argv[4]) != 0)) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }

    int i = 0;

    // Attempt access shared file
    for (; i < 11; ++i) {
        if (i == 10) {
            printf("ERROR_FROM_EX4\n");
            exit(-1);
        }
        // Sleep if file exists
        if (access("to_srv", F_OK) == 0)
            sleep(getrandom());
        else {
            // Create file if file does not exist
            int progID = getpid();
            FILE *f = fopen("to_srv", "w");
            fprintf(f, "%d\n%d\n%d\n%d\n", progID, firstNumber, function, secondNumber);
            fclose(f);
            break;
        }
    }

    alarm(30);
    kill(serverID, SIGUSR1);
    pause();

}
