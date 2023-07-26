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

    // Testing that two clients can speak to each other using SAY RECV
    char buffer[2048] = {0};
    char * identifier1 = "donuts"; // C1: donuts
    char * domain1 = "test_folder";
    char * rd_pipe1 = "test_folder/donuts_RD";
    char * wr_pipe1 = "test_folder/donuts_WR";
    char * identifier2 = "apples"; // C2: apples
    char * domain2 = "test_folder";
    char * rd_pipe2 = "test_folder/apples_RD";
    char * wr_pipe2 = "test_folder/apples_WR";
    
    // CONNECT
    memory_splice(buffer+2, identifier1, 256);
    memory_splice(buffer+2+256, domain1, 1790);
    write(fd, buffer, BUFFER_SIZE);
    memset(buffer, 0, 2048);
    memory_splice(buffer+2, identifier2, 256);
    memory_splice(buffer+2+256, domain2, 1790);
    write(fd, buffer, BUFFER_SIZE);

    sleep(1); // Wait for pipes to be created

    // Whether the named pipes exist
    if (access(rd_pipe1, F_OK) != 0 || access(wr_pipe1, F_OK) != 0 ||
        access(rd_pipe2, F_OK) != 0 || access(wr_pipe2, F_OK) != 0) {
        return 1; 
    } 

    // ________________________________________________________________________

    int rd_fd1 = open(rd_pipe1, O_RDWR); 
    int wr_fd1 = open(wr_pipe1, O_RDWR);
    int rd_fd2 = open(rd_pipe2, O_RDWR); 
    int wr_fd2 = open(wr_pipe2, O_RDWR);

    char * message;
    // client 1 send data, client 2 receive data
    char temp[2048] = {0};
    message = "Hello, this is client 1.";
    memset(buffer, 0, 2048);
    buffer[0] = 0x1;
    strcpy(temp, message);
    memory_splice(buffer+2, temp, 1790);
    write(wr_fd1, buffer, 2048);

    sleep(1);

    read(rd_fd2, buffer, BUFFER_SIZE);
    if (buffer[0] != 0x3) {
        return 1; // Must be RECV type 
    }
    memset(temp, 0, 2048);
    memory_splice(temp, buffer+2, 256);
    if (strcmp(temp, identifier1) != 0) { 
        return 1; // Must be client 1 identifier
    }
    memset(temp, 0, 2048);
    memory_splice(temp, buffer+2+256, 1790);
    if (strcmp(temp, message) != 0) { 
        return 1; // Must be client 1's message
    }

    // ________________________________________________________________________

    // client 2 send data, client 1 receive data
    message = "Received, this is client 2.";
    memset(buffer, 0, 2048);
    memset(temp, 0, 2048);
    buffer[0] = 0x1;
    strcpy(temp, message);
    memory_splice(buffer+2, temp, 1790);
    write(wr_fd2, buffer, 2048);

    sleep(1);

    read(rd_fd1, buffer, BUFFER_SIZE);
    if (buffer[0] != 0x3) {
        return 1; // Must be RECV type 
    }
    memset(temp, 0, 2048);
    memory_splice(temp, buffer+2, 256);
    if (strcmp(temp, identifier2) != 0) { 
        return 1; // Must be client 2 identifier
    }
    memset(temp, 0, 2048);
    memory_splice(temp, buffer+2+256, 1790);
    if (strcmp(temp, message) != 0) { 
        return 1; // Must be client 2's message
    }

    return 0;
}
