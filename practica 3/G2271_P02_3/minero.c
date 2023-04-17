#include "minero.h"
/*
El programa se debe ejecutar con dos parametros:
./ miner < ROUNDS > <LAG >
donde miner es el nombre del ejecutable, <ROUNDS> el numero de rondas que se va a realizar y <LAG> es el retraso en milisegundos entre cada ronda.
El proceso resultante de ejecutar este programa, Minero:
• Creara una cola de mensajes con capacidad para 7 mensajes.
• Establecera un objetivo inicial fijo para la POW (por ejemplo, 0).
• Para cada ronda, resolvera la POW; en el caso de que no se tenga un minero funcional capaz de generar una secuencia de bloques encadenados, se puede utilizar el fichero con la secuencia de 200 rondas proporcionado para ✭✭falsear✮✮ el minero.
• Enviara un mensaje por la cola de mensajes que contenga, al menos, el objetivo y la solucion hallada.
• Realizara una espera de <LAG> milisegundos.
• Establecera como siguiente objetivo la solucion anterior, y repetira la ronda.
• Una vez terminadas las rondas, enviara un bloque especial con algun codigo que permita saber al proceso Comprobador que el sistema esta finalizando.
• Liberara los recursos y terminara.
*/



int pow_search(int objetivo)
{
    int i = 0;
    for (i = 0; i < POW_LIMIT; i++)
    {
        if (pow_hash(i) == objetivo)
        {
            return i;
        }
    }
    return -1;
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s n_rounds lag\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //crea la cola de mensajes
    mqd_t mq;
    msgbuf msg;
    struct mq_attr attr;
    int n_rounds, lag;
    n_rounds = atoi(argv[1]);
    lag = atoi(argv[2]);

    attr.mq_maxmsg = MAX_MSG;
    attr.mq_msgsize = sizeof(msg);
    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;
    //mq_unlink(QUEUE_NAME);
    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0666, &attr);
    if (mq == -1)
    {
        perror("Error creating the message queue");
        exit(EXIT_FAILURE);
    }
 
    
    printf("Generating blocks...\n");
        
    long int objetivo = 0;
    long int res;
    int i;
    for (i = 0; i < n_rounds; i++)
    {
        res = pow_search(objetivo);
        
        /* Enviara un mensaje por la cola de mensajes que contenga, al menos, el objetivo y la solucion hallada. */
        
        msg.obj = objetivo;
        msg.sol = res;
        
        if(i == n_rounds-1)
        {
            msg.flag = 1;
        }
        else
        {
            msg.flag = 0;
        }

        //printf("Enviando mensaje: objetivo = %ld, res = %ld\n", objetivo, res);
        if (mq_send(mq, (char *)&msg, sizeof(msg), 1) == -1)
        {
            mq_close(mq);
            mq_unlink(QUEUE_NAME);
            perror("Error sending message");
            exit(EXIT_FAILURE);
        }
    
        objetivo = res;
        sleep(lag/1000);
    }
    printf("Finished\n");
    
    

    //mq_close(mq);   
    //mq_unlink(QUEUE_NAME);
    exit(EXIT_SUCCESS);
    

}

