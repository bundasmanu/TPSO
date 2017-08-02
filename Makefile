tpso: servidor.c utils.h cliente.c 
	gcc servidor.c -o servidor
	gcc cliente.c -o cliente -lncurses
