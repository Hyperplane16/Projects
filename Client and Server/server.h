#ifndef SERVER_H
#define SERVER_H

#include <stdio.h> // printf()
#include <stdlib.h> // exit()
#include <unistd.h> // pipe() + getpid() + fork() + access() + sleep()
#include <sys/wait.h> // wait()
#include <sys/stat.h> // mfifo() + mkdir() + stat()
#include <sys/time.h> // select struct + sleep()
#include <sys/types.h> // kill()

#include <fcntl.h> // open() + O_WRONLY, O_RDONLY
#include <dirent.h> // filename operations opendir() + readdir()

#include <string.h> // memset()
#include <ctype.h> // isalnum()

#include <signal.h> // sigaction() or signal()
#include <poll.h> // poll

// kill() and sigaction() require -std=gnu11

#define GLOBAL_PIPE "gevent"
#define BUFFER_SIZE 2048

#endif