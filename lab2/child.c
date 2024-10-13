#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>

int id;
bool state;
pid_t pid, ppid;
time_t start_time;

void alarm_handler(int signum) {
    alarm(15); 
    if (kill(pid, SIGUSR1) == -1) {
        perror("kill");
        exit(1);
    }
}

void sigusr1_handler(int signum) {                      
    printf("[ID=%d/PID=%d/TIME=%lds] The gates are %s!\n", id, pid, time(NULL) - start_time, (state) ? "open" : "closed");
}

void sigusr2_handler(int signum){
    state = (!state);
    if (kill(pid, SIGUSR1) == -1) {
        perror("kill");
        exit(1);
    }
}

void sigterm_handler(int signum) { 
    if (kill(ppid, SIGCHLD) == -1) {
        perror("kill");
        exit(1);
    }
    exit(0);
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        return 1;
    }

    id = atoi(argv[1]);
    state = (argv[2][0] == 't');
    pid = getpid();
    ppid = getppid();

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    action.sa_handler = alarm_handler;
    if (sigaction(SIGALRM, &action, NULL) == -1) {
        perror("sigaction");
        return 1;
    }                        

    action.sa_handler = sigusr1_handler;
    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    action.sa_handler = sigusr2_handler;
    if (sigaction(SIGUSR2, &action, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    action.sa_handler = sigterm_handler;
    if (sigaction(SIGTERM, &action, NULL) == -1) {
        perror("sigaction");
        return 1;
    }   

    start_time = time(NULL);
    if (kill(pid, SIGALRM) == -1){
        perror("kill");
    }

    while (1) {
        sleep(1);  
    }

    return 0;
}
