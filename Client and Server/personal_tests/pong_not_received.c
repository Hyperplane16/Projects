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
    char * identifier = "pong";
    char * domain = "test_folder";
    char * rd_pipe = "test_folder/pong_RD";
    char * wr_pipe = "test_folder/pong_WR";

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
    int wr_fd = open(wr_pipe, O_RDWR);

    sleep(15);

    // After 15 seconds the first PING is sent, send back a PONG.
    // PONG
    memset(buffer, 0, 2048);
    buffer[0] = 0x6;
    write(wr_fd, buffer, BUFFER_SIZE);

    // After another 15 seconds the next PING is sent. If a PONG is not
    // sent back, after another 15 seconds the client will be disconnected

    sleep(30);

    // Server should terminate the client pipes
    // Whether the named pipes have been removed
    if (access(rd_pipe, F_OK) == 0 || access(wr_pipe, F_OK) == 0) {
        return 1; 
    }

    return 0;
}
