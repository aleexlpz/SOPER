#include "monitor.h"
#include "comprobador.h"


int monitor(int lag)
{

    ShmStruct *shm = malloc(sizeof(ShmStruct));
    //abre el segmento de memoria compartida
    int fd;
    fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd == -1)
    {
        perror("Error opening the shared memory");
        exit(EXIT_FAILURE);
    }
    //mapea el segmento de memoria compartida
    shm = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED)
    {
        perror("Error mapping the shared memory");
        exit(EXIT_FAILURE);
    }
    //recibe un bloque y lo muestra
    int i = 0;
    printf("Printing blocks...\n");
    while (i < shm->num_slots)
    {
        if (shm->status[i] == 1)
        {
            printf("Solution accepted: %08ld --> %08ld\n", shm->objectives[i], shm->solutions[i]);
            shm->status[i] = 0;
        }
        else if (shm->status[i] == 2)
        {
            printf("Solution rejected: %08ld !-> %08ld\n", shm->objectives[i], shm->solutions[i]);
            shm->status[i] = 0;
        }
        else if (shm->status[i] == 3)
        {
            printf("Solution not evaluated: %08ld !-> %08ld\n", shm->objectives[i], shm->solutions[i]);
            shm->status[i] = 0;
        }
        i++;
        sleep(lag/1000);
    }
    //libera los recursos
    if (munmap(shm, SHM_SIZE) == -1)
    {
        perror("Error unmapping the shared memory");
        exit(EXIT_FAILURE);
    }
    if (close(fd) == -1)
    {
        perror("Error closing the shared memory");
        exit(EXIT_FAILURE);
    }
    if (shm_unlink(SHM_NAME) == -1)
    {
        perror("Error unlinking the shared memory");
        exit(EXIT_FAILURE);
    }
    printf("Finishing\n");
    return 0;
}
