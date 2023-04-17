#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

#include "pow.h"

#define QUEUE_NAME "/cola"
#define MAX_MSG 7
#define BUFFER_SIZE MAX_MSG + 10

typedef struct
{
    long flag;
    int obj;
    int sol;
} msgbuf;

int pow_search(int objetivo);