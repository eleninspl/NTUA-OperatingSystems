#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

int N;
char *state;
bool replace_child = true;
pid_t *child_pid = NULL;
pid_t parent_pid;

bool validString(char *str) {
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] != 't' && str[i] != 'f')
            return false;
    }
    return true;
}

void create_child(int i) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        char index_str[10];
        sprintf(index_str, "%d", i);
        char *args[] = {"./child", index_str, &state[i], NULL};
        execv(args[0], args);
        perror("execv");
        exit(1);
    } else {
        child_pid[i] = pid;
    }
}

void sigterm_handler(int signum) {
    int status;
    replace_child = false;

    for (int i = 0; i < N; i++) {
        printf("[PARENT/PID=%d] Waiting for %d children to exit\n", parent_pid, N - i);
        if (kill(child_pid[i], SIGTERM) == -1){
            perror("kill");
            exit(1);
        }
        waitpid(child_pid[i], &status, 0);
        printf("[PARENT/PID=%d] Child with PID=%d terminated successfully with exit status code %d!\n", parent_pid, child_pid[i], status);
    }
    printf("[PARENT/PID=%d] All children exited, terminating as well\n", parent_pid);
    free(child_pid);
    exit(0);
}

void sigusr1_handler(int signum) {
    for (int i = 0; i < N; i++) {
        if (kill(child_pid[i], SIGUSR1) == -1){
            perror("kill");
            exit(1);
        }
    }
}

void sigchld_handler(int signum) {
    if (replace_child) {
        pid_t pid;
        int child_status;

        if ((pid = waitpid(-1, &child_status, WUNTRACED)) == -1) {
            perror("waitpid");
            exit(1);
        }

        for (int i = 0; i < N; i++) {
            if (child_pid[i] == pid) {
                if (WIFEXITED(child_status)) {
                    printf("[PARENT/PID=%d] Child %d with PID=%d exited\n", parent_pid, i, pid);
                    create_child(i);
                    printf("[PARENT/PID=%d] Created new child for gate %d (PID %d) and initial state '%c' \n", parent_pid, i, child_pid[i], state[i]);
                }
                else if (WIFSTOPPED(child_status)) {
                    printf("[PARENT/PID=%d] Child %d with PID=%d stopped\n", parent_pid, i, pid);
                    if (kill(pid, SIGCONT) == -1) {
                        perror("kill");
                        exit(1);
                    }
                    printf("[PARENT/PID=%d] Child %d with PID=%d resumed\n", parent_pid, i, pid);
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2 || !validString(argv[1])) {
        fprintf(stderr, "Usage: %s <string of (t,f)>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s <string of (t,f)>\n", argv[0]);
        return 0;
    }

    state = argv[1];
    N = strlen(state);
    parent_pid = getpid();
    child_pid = malloc(N * sizeof(pid_t));
    if (child_pid == NULL) {
        perror("malloc");
        return 1;
    }

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    action.sa_handler = sigterm_handler;
    if (sigaction(SIGTERM, &action, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    action.sa_handler = sigusr1_handler;
    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    action.sa_handler = sigchld_handler;
    if (sigaction(SIGCHLD, &action, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    for (int i = 0; i < N; i++) {
        create_child(i);
        printf("[PARENT/PID=%d] Created child %d (PID=%d) and initial state '%c' \n", parent_pid, i, child_pid[i], state[i]);
    }

    while (1) {
        sleep(1);
    }

    return 0;
}
