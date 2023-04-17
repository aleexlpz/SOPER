#ifndef PRACT4_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <mqueue.h>
#define MQ_NAME "/mq_miner"

typedef struct
{
	long int objective;
	long int solution;
	char status;
} Message;

struct mq_attr attributes = {
	.mq_flags = 0,
	.mq_maxmsg = 7,
	.mq_msgsize = sizeof(Message),
	.mq_curmsgs = 0};

#endif
