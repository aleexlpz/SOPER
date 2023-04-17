#include "monitor.h"

int	main(int argc, char *argv[]){
	int	fd_shm;
	int	delay;

	/*Borrable. Por si algo se ga quedado abierto que no debería*/
	/*sem_unlink (SEM_MUTEX);
	sem_unlink (SEM_FILL);
	sem_unlink (SEM_EMPTY);
	shm_unlink(SHM_NAME);*/

	if(argc != 2){
		printf("Numero de argumentos incorrecto\n");
		exit(EXIT_FAILURE);
	}
	delay = atoi(argv[1]);

	fd_shm = shm_open(SHM_NAME, O_RDONLY, 0); //Comprobar si existe la memoria o no
	if (fd_shm == -1)
		comprobador(delay);
	else
		monitor(fd_shm, delay); /*En caso de existir, ya la tengo abierta*/
}

void monitor(int fd_shm, int delay){
	ShmStruct *shm_struct = NULL;
	int i = 0;

	/*Mapeado de la memoria compartida y su control de errores*/
	shm_struct = mmap (NULL, sizeof(ShmStruct), PROT_READ, MAP_SHARED, fd_shm, 0);
	close(fd_shm);
	if (shm_struct == MAP_FAILED){
		perror("mmap");
		shm_unlink(SHM_NAME);
		exit(EXIT_FAILURE);
	}
	//printf("Pointer to shared memory segment : %p\n", ( void *)shm_struct);
	printf("[%d] Printing blocks ...\n", getpid());
	/*Si no se ha escrito nada en la primera posición, no leo nada, de momento*/
	while (shm_struct->status[i]==3);

	/*Printeo lo que hay en el primer slot*/
	if(shm_struct->status[i])
		printf("[%d] Solution accepted: %08ld --> %08ld\n", getpid(), shm_struct->objectives[i], shm_struct->solutions[i]);
	else
		printf("[%d] Solution rejected: %08ld --> %08ld\n", getpid(), shm_struct->objectives[i], shm_struct->solutions[i]);

	printf("[%d] Finishing\n", getpid());
	/*Desmapear la memoria*/
	munmap(shm_struct,sizeof(ShmStruct));
	//shm_unlink(SHM_NAME);
	usleep(delay * 1000); //tiempo de espera introducido en Ms
}

void comprobador(int delay){
	int	fd_shm;
	mqd_t	mq;
	Message msg;
	long int x, y;
	ShmStruct *shm_struct = NULL;
	int i = 0;
	msg.status = 0;

	/*Creo la memoria compartida*/
	if ((fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1){
		perror("shm_open");
		exit(EXIT_FAILURE);
	}
	/*Limito el tamaño necesario*/
	if(ftruncate(fd_shm, sizeof(ShmStruct)) == -1){
		perror("ftruncate");
		shm_unlink(SHM_NAME);
		exit(EXIT_FAILURE);
	}
	/*Mapeado de la memoria compartida y su control de errores*/
	shm_struct = mmap (NULL, sizeof(ShmStruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
	close(fd_shm);
	if (shm_struct == MAP_FAILED){
		perror("mmap");
		shm_unlink(SHM_NAME);
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i < SHM_MAX_SLOTS; i++)
		shm_struct->status[i] = 3; //Para que el otro proceso no lea antes de tiempo

	/*Abro la cola de mensajes*/
	if ((mq = mq_open(MQ_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attributes)) == (mqd_t)-1) {
		perror("mq_open");
		shm_unlink(SHM_NAME);
		exit(EXIT_FAILURE);
	}
	printf("[%d] Checking blocks ...\n", getpid());
	printf("[%d] AVISO: Como monitor puede no funcionar correctamente, yo imprimiré también los mensajes recibidos\n", getpid());
	/*Mientras que no haya llegado a la última ronda (En el caso de que se el caso, recibo un -1 en msg.status)*/
	while(msg.status != -1){
		/*Recibo un mensaje*/
		if (mq_receive(mq, (char *)&msg, sizeof(msg), NULL) == -1) {
			fprintf(stderr, "Error receiving message\n");
			mq_unlink(MQ_NAME);
			shm_unlink(SHM_NAME);
			exit(EXIT_FAILURE);
		}

		/*Si no es el mensaje de fin*/
		if(msg.status != -1){
		/*Recupero los datos*/
		x = msg.solution;
		y = msg.objective;
		msg.status = (y == pow_hash(x)); /*Compruebo que sea valida la solucion*/

		/*Guardo los datos en la memoria compartida*/
		shm_struct->objectives[i%6] = y;
		shm_struct->solutions[i%6] = x;
		shm_struct->status[i%6] = msg.status;
		//printf("[%d] %ld: %ld -- %d\n", getpid(), msg.objective, msg.solution, msg.status); //Lo que he recibido
		printf("[%d] %ld: %ld -- %d\n", getpid(), shm_struct->objectives[i%6], shm_struct->solutions[i%6], shm_struct->status[i%6]); //Lo que meto al SHM
		i++;
		usleep(delay * 1000); //tiempo de espera introducido en Ms
		}
	}
	printf("[%d] Finishing\n", getpid());
	shm_unlink(SHM_NAME);
	exit(EXIT_SUCCESS);
}
