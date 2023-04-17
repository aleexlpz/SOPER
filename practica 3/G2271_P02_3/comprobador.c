/*
El proceso Comprobador:
• Creara e inicializara un segmento de memoria compartida.
• A continuacion, recibira un bloque (a traves de la cola de mensajes, ver "minero.c") y lo comprobara, anadiendole una bandera que indique si es correcto o no.
• Una vez comprobado, lo introducira en memoria compartida para que lo lea Monitor.
• Realizara una espera de <LAG> milisegundos.
• Repetira el proceso de recepcion/comprobacion/escritura hasta que reciba un bloque especial que indique la finalizacion del sistema.
• Cuando reciba el bloque de finalizacion, lo introducira en memoria compartida para notificar al Monitor de la finalizacion del sistema, liberara los recursos y terminara.
*/

#include "comprobador.h"
#include "monitor.h"
#include "minero.h"

int comprobador(int lag)
{
    printf("Comprobador\n");
    // Crea e inicializa un segmento de memoria compartida utilizando la funcion shmget
    int msg_count = 0;
    msgbuf msg;
    ShmStruct *shm = NULL;
    int fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("shm_open");
        exit(1);
    }
    if (ftruncate(fd, sizeof(ShmStruct)) == -1)
    {
        perror("ftruncate");
        shm_unlink(SHM_NAME);
        exit(1);
    }

    printf("Memoria compartida creada\n");

    // Hacer algo con la memoria compartida recién creada
    shm = mmap(NULL, sizeof(ShmStruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (shm == MAP_FAILED)
    {
        perror("mmap");
        shm_unlink(SHM_NAME);
        exit(1);
    }
    for (int i = 0; i < SHM_MAX_SLOTS; i++)
        shm->status[i] = 3;
    /*inicializa la cola de mensajes para recibir un mensaje*/

    mqd_t mq;
    
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_MSG;
    attr.mq_msgsize = sizeof(msgbuf);
    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;
    mq = mq_open(QUEUE_NAME, O_RDONLY, 0666, &attr);

    // Recibe un bloque y lo muestra

    while (msg.flag != 1 || msg_count < MAX_MSG)
    {
        if (mq_receive(mq, (char *)&msg, sizeof(msg), NULL) == -1)
        {
            mq_unlink(QUEUE_NAME);
            shm_unlink(SHM_NAME);
            perror("Error receiving the message");
            exit(EXIT_FAILURE);
        }
        printf("Message received:  %08d --> %08d || %ld\n", msg.value1, msg.value2, msg.flag);
        msg_count++;
        /*inserta en la memoria comparida la informacion*/

        if (msg.flag == 1)
        {
            printf("Finalizando el sistema\n");
            break;
        }
    }

    /*inserta en la memoria comparida la informacion*/
    for (int i = 0; i < SHM_MAX_SLOTS; i++)
        shm->status[i] = 1;

    mq_unlink(QUEUE_NAME);
    

    return 0;

    /*
    if (ftruncate(fd_shm, SHM_SIZE) == -1){
        perror("Error truncating the shared memory");
        exit(EXIT_FAILURE);
    }*/
    // Mapea el segmento de memoria compartida
    /*char *ptr;
    ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    if (ptr == MAP_FAILED){
        perror("Error mapping the shared memory");
        exit(EXIT_FAILURE);
    }
    // Recibe un bloque y lo muestra
    int *puntero = (int *)ptr;
    while (1){
        if (puntero[0] == -1){
            break;
        }
        if (puntero[0] == puntero[1]){
            printf("Solution accepted: %08d --> %08d\n", puntero[0], puntero[1]);
        }else{
            printf("Solution rejected: %08d !-> %08d\n", puntero[0], puntero[1]);
        }
        puntero += 2;
        usleep(lag * 1000);
    }
    // Cierra el segmento de memoria compartida
    if (munmap(ptr, SHM_SIZE) == -1){
        perror("Error unmapping the shared memory");
        exit(EXIT_FAILURE);
    }
    if (close(fd_shm) == -1){
        perror("Error closing the shared memory");
        exit(EXIT_FAILURE);
    }
    if (shm_unlink(SHM_NAME) == -1){
        perror("Error unlinking the shared memory");
        exit(EXIT_FAILURE);
    }*/
}