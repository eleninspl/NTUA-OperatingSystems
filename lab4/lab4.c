#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>      
#include <sys/select.h>
#include <time.h>

#define DEFAULT "\033[30m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"

#define BUFFER_SIZE 1024

char* getEvent(int event){
    switch(event){
        case 0: return "boot";
        case 1: return "setup";
        case 2: return "interval";         
        case 3: return "button";
        case 4: return "motion";
    }
    return "unknown";
}

char* getTime(time_t event_time) {
    struct tm *info;
    info = localtime(&event_time);
    return asctime(info);
}

int main(int argc, char *argv[]) {
	
    bool debug = false;
    char host[] = "os4.iot.dslab.ds.open-cloud.xyz";
    int port = 20241;

    for(int i = 1; i < argc; i++) {
        if (strcmp("--debug", argv[i]) == 0) {
            debug = true;
        }
        if (strcmp("--host", argv[i]) == 0) {
            strncpy(host, argv[++i], sizeof(host));
            continue;
        }
        if (strcmp("--port", argv[i]) == 0) {
            port = atoi(argv[++i]);
            continue;
        }
    }

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);      
    if (sock_fd < 0) { 
        perror("socket");
        return 1; 
    }

    struct sockaddr_in sin;                                                     
    sin.sin_family = AF_INET;

    sin.sin_port = htons(0);                                                    
    sin.sin_addr.s_addr = htonl(INADDR_ANY);                                    
    if (bind(sock_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0){              
        perror("bind");
        return 1;    
    }

    sin.sin_port = htons(port);
    struct hostent *host_address;
    host_address = gethostbyname(host);
    if(host_address == NULL){
        perror("gethostbyname");
        return 1;
    }
    memcpy(&sin.sin_addr, host_address->h_addr_list[0], host_address->h_length);        

    if (connect(sock_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("connect");
        return 1;
    }
    printf(CYAN "Connected!" DEFAULT "\n");

    char buffer[BUFFER_SIZE];
    ssize_t read_bytes;
    fd_set readfds;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        FD_SET(sock_fd, &readfds);
        if (select(sock_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select");
            return 1;
        }

        if (FD_ISSET(0, &readfds)) {
            read_bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE);
            if (read_bytes < 1) {
                perror("read");
                return 1;
            }

            buffer[read_bytes - 1] = '\0'; 
            if (strcmp(buffer, "help") == 0) {
                printf(CYAN "Usage: type 'get' or 'N name surname reason' or 'exit'" DEFAULT "\n");
                continue;
            } 
            if (strcmp(buffer, "exit") == 0) {
                close(sock_fd);
                printf(CYAN "Exiting!" DEFAULT "\n");
                return 0;
            }
            if (write(sock_fd, buffer, read_bytes) < 0) {
                    perror("write");
                    return 1;
                }
            if (debug){
                printf(BLUE "[DEBUG] sent '%s'" DEFAULT "\n", buffer);
            }
            continue;
        }

        if (FD_ISSET(sock_fd, &readfds)) {
            read_bytes = read(sock_fd, buffer, BUFFER_SIZE);
            if (read_bytes < 1) {
                perror("read");
                return 1;
            }
            buffer[read_bytes - 1] = '\0'; 

            if (debug) {
                printf(BLUE "[DEBUG] read '%s'" DEFAULT "\n", buffer);
            }
            if (strcmp(buffer, "try again") == 0 || strcmp(buffer, "invalid code") == 0) {
                printf(CYAN "%s" DEFAULT "\n", buffer);
                continue;
            }

            if (buffer[1] == ' ') {
                int event, light, temperature, event_time;
                sscanf(buffer, "%d %d %d %d", &event, &light, &temperature, &event_time);

                printf(CYAN "---------------------------\n");
                printf("Latest event:\n");
                printf("%s (%d)\n", getEvent(event), event);
                printf("Temperature is: %.2f\n", temperature/100.0);
                printf("Light level is: %d\n", light);
                printf("Timestamp is: %s", getTime(event_time));
                printf("---------------------------" DEFAULT "\n");
                continue;
            }

            if (strncmp(buffer, "ACK", 3) == 0) {
                buffer[sizeof(buffer) - 1] = '\0'; 
                printf(CYAN "Response: '%s' " DEFAULT "\n", buffer);
                continue;
            }

            printf(CYAN "Send verification code : '%s'" DEFAULT "\n", buffer);
        }
    }

    close(sock_fd);
	return 0;
}
