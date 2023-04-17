#include <sys/mman.h>
#include "pow.h"
#include <semaphore.h>
#include "Prac4.h"

#define SHM_NAME "/shm_codeBlocks"
#define SEM_MUTEX "/sem_Mutex"
#define SEM_FILL "/sem_Fill"
#define SEM_EMPTY "/sem_Empty"
#define SHM_MAX_SLOTS 6

typedef struct {
	long int	objectives[SHM_MAX_SLOTS];
	long int	solutions[SHM_MAX_SLOTS];
	char	status[SHM_MAX_SLOTS];
} ShmStruct;

void comprobador(int delay);
void monitor(int fd_shmm, int delay);
