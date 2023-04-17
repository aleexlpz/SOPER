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
#include "comprobador.h"
#include "monitor.h"

#define INT_LIST_SIZE 10
#define MSG_MAX 100

typedef struct
{
    int integer_list[INT_LIST_SIZE];
    char message[MSG_MAX];
} ShmExampleStruct;

int main(int argc, char *argv[])
{    
    int lag = atoi(argv[1]);

    int fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (fd < 0)
    {
        // La memoria compartida no existe
        comprobador(lag);
    }
    else
    {
        // La memoria compartida ya existe
        monitor(lag);
    }


    
    return 0;
}