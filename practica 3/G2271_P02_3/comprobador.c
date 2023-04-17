#include "comprobador.h"
#include "monitor.h"
#include "minero.h"
#include "pow.h"

int comprobador(int lag)
{
    // Crea e inicializa un segmento de memoria compartida utilizando la funcion shmget
    int msg_count = 0;
    msgbuf msg;
    ShmStruct *shm = malloc(sizeof(ShmStruct));
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


    // Hacer algo con la memoria compartida recién creada
    shm = mmap(NULL, sizeof(ShmStruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (shm == MAP_FAILED)
    {
        perror("mmap");
        shm_unlink(SHM_NAME);
        exit(1);
    }
    /*inicializa la cola de mensajes para recibir un mensaje*/
    for (int i = 0; i < SHM_MAX_SLOTS; i++)
        shm->status[i] = 3;
    mqd_t mq;
    
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_MSG;
    attr.mq_msgsize = sizeof(msgbuf);
    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;
    mq = mq_open(QUEUE_NAME, O_RDONLY, 0666, &attr);

    int i = 0;
    printf("Checking blocks...\n");
    while (msg.flag != 1 || msg_count < MAX_MSG)
    {
        if (mq_receive(mq, (char *)&msg, sizeof(msg), NULL) == -1)
        {
            mq_unlink(QUEUE_NAME);
            shm_unlink(SHM_NAME);
            perror("Error receiving the message");
            exit(EXIT_FAILURE);
        }
        //printf("Message received:  %08d --> %08d || %ld\n", msg.obj, msg.sol, msg.flag);
        msg_count++;
        if(pow_hash(msg.sol) == msg.obj)
        {
            shm->status[i] = 1;
        }
        else
        {
            shm->status[i] = 2;
        }
        shm->objectives[i] = msg.obj;
        shm->solutions[i] = msg.sol;
        i++;
        shm->num_slots = i;
        sleep(lag/1000);

        if (msg.flag == 1)
        {
            printf("Finishing\n");
            break;
        }
    }

    mq_unlink(QUEUE_NAME);

    return 0;
}