#include "server.h"

int main(int argc, char ** argv) {
    char * global_pipe = GLOBAL_PIPE;
    unlink(global_pipe);
    return 0;
}
