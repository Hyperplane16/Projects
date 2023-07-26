#include "../server.h"

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

    // Testing that the first ping in the client is received after 15 seconds
    char buffer[2048] = {0};
    char * identifier = "ping";
    char * domain = "test_folder";
    char * rd_pipe = "test_folder/ping_RD";
    char * wr_pipe = "test_folder/ping_WR";

    // CONNECT
    memory_splice(buffer+2, identifier, 256);
    memory_splice(buffer+2+256, domain, 1790);
    write(fd, buffer, BUFFER_SIZE);

    sleep(1); // Wait for pipes to be created

    // Whether the named pipes exist
    if (access(rd_pipe, F_OK) != 0 || access(wr_pipe, F_OK) != 0) {
        return 1; 
    } 

    // ________________________________________________________________________
    
    int rd_fd = open(rd_pipe, O_RDWR); 
    int wr_fd = open(wr_pipe, O_WRONLY);

    // Test that no data is sent within the first 15 seconds (ping is not premature)
    // time() structures
    int counter = 0;
    struct pollfd poll_list[1];
    poll_list[0].fd = rd_fd;
    poll_list[0].events = POLLIN; // POLLIN (read)
    nfds_t nfds = 1;
    while (1) {
        // Test data is available to be read while non-blocking
        int ret = poll(poll_list, nfds, 1); // (delay in milliseconds)
            
        if ((poll_list[0].revents&POLLIN) == POLLIN) { 
            return 1; // Data exists prematurely (before 15 seconds)
        }

        sleep(1);
        counter += 1;
        if (counter == 13) { 
            break; // Break before the anticipated ping for reading
        }
    }

    // Accept Pong
    memset(buffer, 0, 2048);
    read(rd_fd, buffer, 2048);
    if (buffer[0] != 0x5) {
        return 1; // Did not receive a PING type message
    }

    return 0;
}