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

    // Testing a standard DISCONNECT removes the pipes
    char identifier[256] = "disconnection";
    char domain[1790] = "test_folder";
    char rd_pipe[1790+256+1+3] = "test_folder/disconnection_RD";
    char wr_pipe[1790+256+1+3] = "test_folder/disconnection_WR";
    char buffer[2048] = {0};

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
    
    sleep(1);

    // DISCONNECT
    memset(buffer, 0, 2048);
    buffer[0] = 0x7;
    int wr_fd = open(wr_pipe, O_RDWR);
    write(wr_fd, buffer, BUFFER_SIZE);

    sleep(1);
    // Whether the named pipes have been removed
    if (access(rd_pipe, F_OK) == 0 || access(wr_pipe, F_OK) == 0) {
        return 1; 
    }

    return 0;
}
