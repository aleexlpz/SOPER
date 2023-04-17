NAME=miner monitor

all: $(NAME)

monitor:
	gcc -c -Wall -Wextra -Werror pow.c -o pow.o
	gcc -c -Wall -Wextra -Werror monitor.c -o monitor.o
	gcc -o monitor pow.o monitor.o -lrt -lpthread

miner:
	gcc -c -Wall -Wextra -Werror miner.c -o miner.o
	gcc -o miner miner.o -lrt

re: fclean all

fclean: clean
	rm -rdf $(NAME)
clean:
	rm -rdf miner.o monitor.o pow.o
