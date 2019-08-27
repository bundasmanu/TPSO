tpso: servidor.c  cliente.c arbitro.c utils.h
	gcc servidor.c -o servidor -lncurses -pthread
	gcc cliente.c -o cliente -lncurses -pthread
	gcc arbitro.c -o arbitro
