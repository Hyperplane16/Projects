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

    // Testing that a very long nested set of directories can be created
    char * identifier = "domain_connections";
    char * domain = "test_folder/nested/another_nested/another_one";
    char * rd_pipe = "test_folder/nested/another_nested/another_one/domain_connections_RD";
    char * wr_pipe = "test_folder/nested/another_nested/another_one/domain_connections_WR";
    char buffer[2048] = {0};

    // CONNECT
    memory_splice(buffer+2, identifier, 256);
    memory_splice(buffer+2+256, domain, 1790);
    write(fd, buffer, BUFFER_SIZE);

    sleep(1); // Wait for pipes to be created

    // Whether the named pipes exist
    if (access(rd_pipe, F_OK) != 0 || access(wr_pipe, F_OK) != 0) {
        return 0; 
    }

    // ________________________________________________________________________

    // Testing identifier name with illegal characters
    identifier = "domain_/connec\t/ions";
    domain = "test_folder";
    rd_pipe = "test_folder/domain_/connections_RD";
    wr_pipe = "test_folder/domain_/connections_WR";
    memset(buffer, 0, 2048);

    // CONNECT
    memory_splice(buffer+2, identifier, 256);
    memory_splice(buffer+2+256, domain, 1790);
    write(fd, buffer, BUFFER_SIZE);

    sleep(1); // Wait for pipes to be created

    // Whether that the named pipes don't exist
    if (access(rd_pipe, F_OK) == 0 || access(wr_pipe, F_OK) == 0) {
        return 1;
    }

    return 0;
}
