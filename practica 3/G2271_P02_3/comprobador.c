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

struct msgbuf
{
    int value1;
    int value2;
} msg_buf;


int comprobador(int lag){
    printf("Comprobador\n");
    // Crea e inicializa un segmento de memoria compartida utilizando la funcion shmget
    int msg_count = 0;
    int fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        perror("shm_open");
        exit(1);
    }
    ftruncate(fd, 1024);
    printf("Memoria compartida creada\n");
    
    // Hacer algo con la memoria compartida recién creada
    void *ptr = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    /*inicializa la cola de mensajes para recibir un mensaje*/
    mqd_t mq;

    mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (!mq)
    {
        perror("Error opening the message queue");
        exit(EXIT_FAILURE);
    }
    // Recibe un bloque y lo muestra
    struct msgbuf msg;
    while(msg_count < MAX_MSG){
        if (!mq_receive(mq, (char *)&msg, BUFFER_SIZE, NULL))
        {
            perror("Error receiving the message");
            exit(EXIT_FAILURE);
        }
        printf("Message received:  %08d --> %08d\n", msg.value1, msg.value2);
        msg_count++;      
    }
    mq_close(mq);
    // Cierra el segmento de memoria compartida
    if (munmap(ptr, 1024) == -1)
    {
        perror("Error unmapping the shared memory");
        exit(EXIT_FAILURE);
    }
    /*elimina la memoria reservad*/
    close( fd );
    if (shm_unlink(SHM_NAME) == -1)
    {
        perror("Error removing the shared memory");
        exit(EXIT_FAILURE);
    }

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
    return 0;
}