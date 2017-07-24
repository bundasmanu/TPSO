#include "utils.h"
#include<ncurses.h>


void main(){

    initscr();//inicializar a variavel global stdscr

    raw();
    move(10,10);  //Aqui estamos movendo o cursor para a linha 2 coluna 1.
    printw("Olá mundo!!!");  //Imprimimos um texto na tela na posição acima.
    refresh();			/* Print it on to the real screen */
	getch();			/* Wait for user input */

    endwin();

    exit(0);
}
