#include "server.h"

// Copy a "length" of data in one memory location to another memory location
// (Ensure that length is within both data structures)
void memory_splice(char * start, char * string, int length) {
    for (int i = 0; i < length; i++) {
        start[i] = string[i];
    }
}

int main(int argc, char ** argv) {
    char * global_pipe = GLOBAL_PIPE;
    int fd = open(global_pipe, O_WRONLY);
    if (fd == -1) {
        printf("Global named pipe %s cannot be opened.\n", global_pipe);
        return 1;
    }
    
    char buffer[BUFFER_SIZE] = {0};
    char operation[32] = {0};
    char identifier[256] = {0};
    char domain[1790] = {0};
    char rd_pipe[256+1790+1+3] = {0}; 
    char wr_pipe[256+1790+1+3] = {0};
    char path[256+1790] = {0};

    // CONNECT Protocol
    while (1) {
        if (read(STDIN_FILENO, buffer, BUFFER_SIZE) <= 0) {
            continue;
        }
        sscanf(buffer, "%s %s %s", operation, identifier, domain);
        memset(buffer, 0, BUFFER_SIZE);

        // CONNECT <0 0> <identifier 256> <domain 1790>
        if (strcmp(operation, "CONNECT") == 0) {
            memory_splice(buffer+2, identifier, 256);
            memory_splice(buffer+2+256, domain, 1790);
            
            strcat(path, domain);
            strcat(path, "/");
            strcat(path, identifier);
            memory_splice(rd_pipe, path, strlen(path));
            strcat(rd_pipe, "_RD");
            memory_splice(wr_pipe, path, strlen(path));
            strcat(wr_pipe, "_WR");
            write(fd, buffer, BUFFER_SIZE);
            break;
        }
        printf("[System] Input a CONNECT protocol\n");
    }
    close(fd);

    // ________________________________________________________________________

    sleep(1); // Ensures the pipes are constructed before they are opened

    // Use O_RDWR to prevent read from blocking as the server has opened it yet
    int rd_fd = open(rd_pipe, O_RDWR); 
    int wr_fd = open(wr_pipe, O_WRONLY);
    if (rd_fd == -1 || wr_fd == -1) {
        printf("Client: Named pipe of identifier %s cannot be opened.\n", identifier);
        return 1;
    }
    
    fd_set rfds;
    struct timeval tv;
    int nfds = rd_fd+1;
    long delay_seconds = 1;
    char first_field[BUFFER_SIZE];
    char second_field[BUFFER_SIZE];
    char third_field[BUFFER_SIZE];
    char fourth_field[BUFFER_SIZE];
    while (1) {
        // Reinitialise each file descriptor set each loop (required)
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        FD_SET(rd_fd, &rfds);
		tv.tv_sec = delay_seconds;
		tv.tv_usec = 0;

        // After select returns, fds that are not ready for access are cleared
        int ret = select(nfds, &rfds, NULL, NULL, &tv);
		if (ret == -1) {
			printf("Client failed to access named pipes");
            return 1;
        } else if (ret) { // If at least one descriptor is accessable

            // User input exists
            if (FD_ISSET(0, &rfds)) { 
                memset(buffer, 0, BUFFER_SIZE);
                if (read(STDIN_FILENO, buffer, BUFFER_SIZE) <= 0) {
                    continue;
                }
                sscanf(buffer, "%s %s %s %s", first_field, second_field, 
                    third_field, fourth_field);
                
                // SAY <1 0> <message 1790>
                if (strcmp(first_field, "SAY") == 0) {
                    char temp[BUFFER_SIZE] = {0};
                    strcpy(temp, buffer);
                    memset(buffer, 0, BUFFER_SIZE);
                    buffer[0] = 0x1;
                    memory_splice(buffer+2, temp+4, 1790); 
                    // temp+4 is after "SAY "

                // SAYCONT <2 0> <message 1789> <termination 2046th>
                } else if (strcmp(first_field, "SAYCONT") == 0) {
                    char temp[BUFFER_SIZE] = {0};
                    strcpy(temp, buffer);
                    memset(buffer, 0, BUFFER_SIZE);
                    buffer[0] = 0x2;
                    memory_splice(buffer+2, temp+8, 1789); 
                    // temp+8 is after "SAYCONT "

                    printf("[SYSTEM] Input Termination: ");
                    unsigned char termination;
                    scanf("%hhu", &termination);
                    buffer[2048-1] = termination;

                    // Clear Standard Input Buffer
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF) { }
                    
                // DISCONNECT <7 0> 
                } else if (strcmp(first_field, "DISCONNECT") == 0) { 
                    memset(buffer, 0, BUFFER_SIZE);
                    buffer[0] = 0x7;
                    write(wr_fd, buffer, BUFFER_SIZE);
                    close(rd_fd);
                    close(wr_fd);
                    return 0;

                // No applicable protocol type
                } else {
                    printf("[SYSTEM] Invalid Protocol\n");
                    continue;
                }

                write(wr_fd, buffer, BUFFER_SIZE);
            
            // Data written from the server exists
            } if (FD_ISSET(rd_fd, &rfds)) {
                memset(buffer, 0, BUFFER_SIZE);
                // UNFIXED: For some reason even when select clears it for reading
                // read() still returns <= 0, and the data is all '\0'
                // - REASON: caused by rd_fd, which somehow bypasses select()
                if (read(rd_fd, buffer, BUFFER_SIZE) <= 0) {
                    continue;
                }

                // PING received, send back PONG
                if (buffer[0] == 0x5 && buffer[1] == 0x0) {
                    // printf("[System] Ping Received\n");
                    char pong_protocol[2048] = {0};
                    pong_protocol[0] = 0x6; 
                    write(wr_fd, pong_protocol, 2048);
                    continue;
                }

                printf("[System] Data Received\n");
                for (int i = 0; i < BUFFER_SIZE; i++) {
                    printf("%c", buffer[i]);
                }
                printf("\n");
            }
        } 
    }
    return 0;
}
