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

    // Testing invalid data to gevent
    char identifier[256] = "malform";
    char domain[1790] = "test_folder";
    char rd_pipe[1790+256+1+3] = "test_folder/malform_RD";
    char wr_pipe[1790+256+1+3] = "test_folder/malform_WR";
    char buffer[2048] = {0};

    // CONNECT
    memory_splice(buffer+2, identifier, 256);
    memory_splice(buffer+2+256, domain, 1790);
    buffer[1] = 0x1;
    write(fd, buffer, BUFFER_SIZE);

    sleep(1); // Wait for pipes to be created

    // The named pipes should NOT exist (poor type)
    if (access(rd_pipe, F_OK) == 0 || access(wr_pipe, F_OK) == 0) {
        return 1; 
    }

    // Writing SAY type without connecting
    char temp[2048] = {0};
    char * message = "This is a premature SAY message before connection";
    memset(buffer, 0, 2048);
    buffer[0] = 0x1;
    strcpy(temp, message);
    memory_splice(buffer+2, temp, 1790);
    int wr_fd = open(wr_pipe, O_WRONLY | O_NONBLOCK);
    write(wr_fd, buffer, 2048);

    // The named pipes still should NOT exist (no connection)
    if (access(rd_pipe, F_OK) == 0 || access(wr_pipe, F_OK) == 0) {
        return 1; 
    }
    
    return 0;
}
