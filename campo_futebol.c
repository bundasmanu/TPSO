#include "utils.h"
#include<ncurses.h>
#include<curses.h>

void inicializa_ncurses(){

initscr();//inicializar a variavel global stdscr

    start_color();
    clear();
    cbreak();
    //noecho();

}


void desenhaCampo(WINDOW *win) {
    int i,j;
    for(i=0;i<4;i++){
        for(j=0;j<30;j++){
            if(i==0||i==3){
                mvaddch(i,j,'-');//so na 1 e ultima linha é que se mete se '-'
            }
            if(j==0||j==(30-1)){
                 mvaddch(i,j,'|');//na 1 e ultima coluna metesse '|'
            }
        }
    }

    for(i=0;i<4;i++){
        for(j=31;j<61;j++){
            if(i==0 || i==3){
                mvaddch(i,j,'-');
            }
            if(j==31||j==(61-1)){
                mvaddch(i,j,'|');
            }
        }
    }

    wrefresh(win);
}


void main(){
WINDOW *win_consola;
char msg[50];

    printf("lalalla.\n");
    sleep(2);
    inicializa_ncurses();
    init_pair(1,COLOR_WHITE,COLOR_BLACK);
    wattrset(win_consola,COLOR_PAIR(1));
    scrollok(win_consola,TRUE);
    wbkgd(win_consola,COLOR_PAIR(1));
    wclear(win_consola);
    wrefresh(win_consola);
            desenhaCampo(win_consola);
    wrefresh(win_consola);
    win_consola=newwin(51,21,10,5);
    wgetch(win_consola);
    endwin();
    while(1){
        printf("lalalla.\n");
        scanf(" %s",msg);
        printf("Escreveste: %s\n",msg);
        }
return;
}
