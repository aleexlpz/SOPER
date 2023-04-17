#include "Prac4.h"

void enviarMensaje(mqd_t queue, Message msg, FILE *file);

int	main(int argc, char *argv[]){

	mqd_t		queue;
	unsigned int	nRounds = 0, delay = 0;
	long int		round_obj, round_sol;
	FILE	*file;
	Message	msg;

	/*Compruebo los argumentos*/
	if(argc != 3){
		printf("Numero de argumentos incorrecto\n");
		exit(EXIT_FAILURE);
	}
	nRounds = atoi(argv[1]);
	if(nRounds > 500){
		printf("No se pueden soportar tantas rondas\n");
		exit(EXIT_FAILURE);
	}
	delay = atoi(argv[2]);
	file = fopen("mrush_sequence.dat", "r"); //Cargo el minero
	if (file == NULL)
		exit(EXIT_FAILURE);

	/*Crear la cola de mensajes*/
	queue = mq_open(MQ_NAME, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR, &attributes);
	if (queue < 0){
		fclose(file);
		exit(EXIT_FAILURE);
	}

	fscanf(file, "%ld\n", &round_obj); //Recibo el primer objetivo
	printf("[%d] Generating blocks ...\n", getpid());

	/*Durante el numero de rondas*/
	for (unsigned int i = 0; i < nRounds; i++){
		fscanf(file, "%ld\n", &round_sol);
		/*Preparo los datos para el envío*/
		msg.objective = round_obj;
		msg.solution  = round_sol;
		msg.status = 0;
		/*Envío el mensaje*/
		enviarMensaje(queue, msg, file);
		round_obj = round_sol;/*Preparo un nuevo objetivo*/
		usleep(delay * 1000);
	}
	msg.status = -1;
	enviarMensaje(queue, msg, file);
	mq_close(queue);
	mq_unlink(MQ_NAME);
	printf("[%d] Finishing\n", getpid());
	fclose(file);
	exit(EXIT_SUCCESS);
}

void enviarMensaje(mqd_t queue, Message msg, FILE *file){
	if(mq_send(queue,(char *)&msg, sizeof(msg), 1) == -1) {
		fprintf(stderr, "Error sending message\n");
		fclose(file);
		mq_close(queue);
		exit(EXIT_FAILURE);
	}
}
