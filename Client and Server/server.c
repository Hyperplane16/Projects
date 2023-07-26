#include "server.h"

// Signal handler
void sig_handler(int signum) {
    wait(NULL);
}

// Copy a "length" of data in one memory location to another memory location
// (Ensure that length is within both data structures)
void memory_splice(char * start, char * string, int length) {
    for (int i = 0; i < length; i++) {
        start[i] = string[i];
    }
}

void is_sanitary_filename(char * name) {
    for (int i = 0; i < strlen(name); i++) {
        if (name[i] == '_' || name[i] == '/') {
            continue;
        }
        if (isalnum(name[i]) == 0) {
            printf("The string %s contains non-alphanumeric characters\n", name);
            exit(1);
        }
    }
}

void write_to_clients(char * buffer, char * domain, char * rd_pipe) {
    DIR * dir;
    struct dirent * dir_struct;
    dir = opendir(domain);
    int fd;
    if (dir) {   
        while ((dir_struct = readdir(dir)) != NULL) {  
            
            char path[256+1+1790]= {0};
            strcat(path, domain);
            strcat(path, "/");
            strcat(path, dir_struct->d_name);

            if (path[strlen(path)-1] == 'D' && path[strlen(path)-2] == 'R' 
                && path[strlen(path)-3] == '_') { // Suffixed by _RD
                if (strcmp(path, rd_pipe) == 0) {
                    continue; // Don't write if it is the current client
                }
                fd = open(path, O_WRONLY);
                if (fd == -1) {
                    continue;
                }
                
                write(fd, buffer, BUFFER_SIZE);
                close(fd);
            }
        }
        closedir(dir);
    }
}

// A PING will be initially sent after 15 seconds, then every 15
// seconds. If at the sending of a PING the previous PONG has not
// been received, the client is disconnected (except for first ping)
int handle_ping_pong(struct timeval * time_now, struct timeval * time_initial, 
    int * pong_received, char * rd_pipe, char * wr_pipe) {
    double elapsed_time;
    gettimeofday(time_now, NULL); // Calculate elapsed time
    elapsed_time = (time_now->tv_sec - time_initial->tv_sec);
    if (elapsed_time >= 15) {
        gettimeofday(time_initial, NULL); // Reset the timer to 0
        if (* pong_received == 0) { // End the client handler
            unlink(rd_pipe);
            unlink(wr_pipe);
            kill(getppid(), SIGUSR1);
            return 0;
        
        // Send a PING message back to the client
        } else if (* pong_received == 1) {
            * pong_received = 0;
            // Ignore open errors (will be caught later), O_NONBLOCK to stop
            // perpetual blocking if the client's pipe end closes.
            int rd_fd = open(rd_pipe, O_WRONLY | O_NONBLOCK);
            char ping_protocol[2048] = {0};
            ping_protocol[0] = 0x5; 
            write(rd_fd, ping_protocol, 2048);
            close(rd_fd);
        }
    }
    return 1;
}

int initialise_client_connection(char * buffer, char * identifier, char * domain,
    char * rd_pipe, char * wr_pipe) { 
    memory_splice(identifier, buffer+2, 256);
    memory_splice(domain, buffer+2+256, 1790);

    // Create the domain. If an error arises check if it already exists,
    // if not, then the domain cannot be created.
    int ret = mkdir(domain, 0777);
    if (ret == -1) {
        struct stat st = {0};
        if (stat(domain, &st) == -1) { // If domain doesn't already exist
            printf("Domain %s cannot be created.\n", domain);
            return 0;
        }
    }

    char form[256+1790+1] = {0};
    strcat(form, domain);
    strcat(form, "/");
    strcat(form, identifier);
    strcat(rd_pipe, form);
    strcat(rd_pipe, "_RD");
    strcat(wr_pipe, form);
    strcat(wr_pipe, "_WR");

    if (mkfifo(rd_pipe, 0666) == -1 || mkfifo(wr_pipe, 0666) == -1) { 
        printf("Named pipe %s cannot be created.\n", identifier);
        return 0;
    }
    return 1;
}

// ----------------------------------------------------------------------------

int main(int argc, char ** argv) {
    signal(SIGUSR1, sig_handler); // Register signal handler

    // Create the global named pipe in the current directory
    char * global_pipe = GLOBAL_PIPE;
    unlink(global_pipe);
    if (mkfifo(global_pipe, 0666) == -1) { // 0666 gives r+w permission
        printf("Global named pipe %s cannot be created.\n", global_pipe);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    char identifier[256];
    char domain[1790];
    char rd_pipe[1790+256+1+3];
    char wr_pipe[1790+256+1+3];
    while (1) {
        // (Placed within loop or else read() does not block)
        int fd = open(global_pipe, O_RDONLY);
        if (fd == -1) {
            printf("Global named pipe %s cannot be read.\n", global_pipe);
            return 1;
        }
        
        memset(buffer, 0, BUFFER_SIZE);
        // ISSUE: When a client CLTR-C before connection a read occurs
        if (read(fd, buffer, BUFFER_SIZE) <= 0) {
            continue;
        }
        if (buffer[0] != 0x0 || buffer[1] != 0x0) { 
            continue; // gevent data must be the CONNECT protocol
        }

        // ____________________________________________________________________

        // CONNECT <type 2> <identifier 256> <domain 1790>
        memset(identifier, 0, 256);
        memset(domain, 0, 1790);
        memset(rd_pipe, 0, sizeof(rd_pipe));
        memset(wr_pipe, 0, sizeof(wr_pipe));
        if (initialise_client_connection(buffer, identifier, domain, rd_pipe, 
            wr_pipe) == 0) {
            continue;
        }
        memset(buffer, 0, BUFFER_SIZE);
        close(fd);
        
        // ____________________________________________________________________

        pid_t pid = fork();
        if (pid < 0) {
            printf("Client handler process has failed at fork.\n");
            continue;
        
        // CLIENT HANDLER PROCESS
        } else if (pid == 0) {
            // Client handler reads from the write pipe of the client
            int wr_fd = open(wr_pipe, O_RDONLY);
            if (wr_fd == -1) {
                printf("Named pipe %s cannot be read.\n", wr_pipe);
                return 1;
            }  

            // STRUCTURE INITIALISATION 
            // poll() structures
            struct pollfd poll_list[1];
            poll_list[0].fd = wr_fd;
            poll_list[0].events = POLLIN; // POLLIN (read)
            nfds_t nfds = 1;

            // time() structures
            struct timeval time_initial, time_now;
            gettimeofday(&time_initial, NULL); // Initialise timer

            // ________________________________________________________________

            int pong_received = 1; // Whether PONG is received (starts as true)
            char message[1790];
            while (1) {
                if (handle_ping_pong(&time_now, &time_initial, &pong_received, 
                    rd_pipe, wr_pipe) == 0) {
                    printf("A PONG response has not been recevied within 15 seconds");
                    printf(" and the client %s will be disconnected.\n", identifier);
                    close(wr_fd);
                    return 0;
                }

                // Test data is available to be read while non-blocking
                int ret = poll(poll_list, nfds, 500); // (delay in milliseconds)
                if (ret < 0) {
                    printf("Error with polling client read file descriptor\n");
                    return 1;
                }
                if ((poll_list[0].revents&POLLIN) != POLLIN) { 
                    continue; // If data exists, interpret the data
                }

                // ____________________________________________________________
                
                memset(buffer, 0, BUFFER_SIZE); 
                memset(message, 0, 1790);
                read(wr_fd, buffer, BUFFER_SIZE);

                // SAY <type: 1 0> <message 1790>
                if (buffer[0] == 0x1 && buffer[1] == 0x0) {
                    memory_splice(message, buffer+2, 1790);

                    // RECEIVE <type: 3 0> <identifier 256> <message 1790>
                    memset(buffer, 0, BUFFER_SIZE);
                    buffer[0] = 0x3;
                    memory_splice(buffer+2, identifier, 256);
                    memory_splice(buffer+2+256, message, 1790);
                    write_to_clients(buffer, domain, rd_pipe);

                // SAYCONT <type: 2 0> <message 1789> <termination 2046th>
                } else if (buffer[0] == 0x2 && buffer[1] == 0x0) {
                    memory_splice(message, buffer+2, 1789);

                    char termination = buffer[2048-1];
                    // RECVCONT <type: 4 0> <identifier 256> <message 1789> <term 1>
                    memset(buffer, 0, BUFFER_SIZE);
                    buffer[0] = 0x4;
                    memory_splice(buffer+2, identifier, 256);
                    memory_splice(buffer+2+256, message, 1789);
                    buffer[2048-1] = termination;
                    write_to_clients(buffer, domain, rd_pipe);

                // DISCONNECT <type: 70>
                } else if (buffer[0] == 0x7 && buffer[1] == 0x0) {
                    unlink(rd_pipe);
                    unlink(wr_pipe);
                    close(wr_fd);
                    kill(getppid(), SIGUSR1);
                    return 0;
 
                // PONG <type: 6 0>
                } else if (buffer[0] == 0x6 && buffer[1] == 0x0) {
                    pong_received = 1;
                }
            }
        }
    }
    return 0;
}
