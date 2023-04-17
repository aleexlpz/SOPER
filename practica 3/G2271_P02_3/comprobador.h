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

#define SHM_MAX_SLOTS 6

typedef struct
{
    long int objectives[SHM_MAX_SLOTS];
    long int solutions[SHM_MAX_SLOTS];
    char status[SHM_MAX_SLOTS];
} ShmStruct;


int comprobador(int lag);