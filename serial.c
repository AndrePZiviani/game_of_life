/*
 * The Game of Life
 *
 * a cell is born, if it has exactly three neighbours 
 * a cell dies of loneliness, if it has less than two neighbours 
 * a cell dies of overcrowding, if it has more than three neighbours 
 * a cell survives to the next generation, if it does not die of loneliness 
 * or overcrowding 
 *
 * In my version, a 2D array of ints is used.  A 1 cell is on, a 0 cell is off.
 * The game plays 100 rounds, printing to the screen each time.  'x' printed
 * means on, space means 0.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int linha,coluna,**board,*dboard,**newboard,*dnboard;
/* set everthing to zero */

void initialize_board (int **board) {
    int i, j;

    for (i=0; i<linha; i++) 
        for (j=0; j<coluna; j++) 
            board[i][j] = rand() < RAND_MAX / 10 ? 1 : 0;
}

/* add to a width index, wrapping around like a cylinder */

int xadd (int i, int a) {
    i += a;
    if (i < 0) i += linha;
    if (i >= linha) i -= linha;
    return i;
}

/* add to a height index, wrapping around */

int yadd (int i, int a) {
    i += a;
    if (i < 0) i += coluna;
    if (i >= coluna) i -= coluna;
    return i;
}

/* return the number of on cells adjacent to the i,j cell */

int adjacent_to (int **board, int i, int j) {
    int k, l, count;

    count = 0;

    /* go around the cell */

    for (k=-1; k<=1; k++)
        for (l=-1; l<=1; l++)
            if (k || l)
                if (board[xadd(i,k)][yadd(j,l)]) count++;
    return count;
}

void play (int **board) {
/*
    (copied this from some web page, hence the English spellings...)

    1.STASIS : If, for a given cell, the number of on neighbours is 
    exactly two, the cell maintains its status quo into the next 
    generation. If the cell is on, it stays on, if it is off, it stays off.

    2.GROWTH : If the number of on neighbours is exactly three, the cell 
    will be on in the next generation. This is regardless of the cell's         current state.

    3.DEATH : If the number of on neighbours is 0, 1, 4-8, the cell will 
    be off in the next generation.
*/
    int i, j, a;

    /* for each cell, apply the rules of Life */

    for (i=0; i<linha; i++)
        for (j=0; j<coluna; j++) {
            a = adjacent_to (board, i, j);
            if (a == 2) newboard[i][j] = board[i][j];
            if (a == 3) newboard[i][j] = 1;
            if (a < 2) newboard[i][j] = 0;
            if (a > 3) newboard[i][j] = 0;
        }

    /* copy the new board back into the old board */

}

/* print the life board */

void print (int **board) {
    int i, j;

    /* for each row */

    for (i=0; i<linha; i++) {

        /* print each column position... */

        for (j=0; j<coluna; j++) {
            printf ("%c", board[i][j] ? 'x' : ' ');
        }

        /* followed by a carriage return */

        printf ("#");
        printf ("\n");
    }
}

void read_file (int **board, char *name) {
    FILE    *f;
    int i, j;
    char    s[110],c;

    f = fopen (name, "r");
    for (i=0; i<linha; i++) {

        /* get a string */

        for (j=0; j<coluna; j++) {
            c=fgetc(f);
            board[i][j] = c == 'x';
        }
            c=fgetc(f);
            if (c == '#')
                c=fgetc(f);//tira /n
    }
    fclose (f);
}

/* main program */

int main (int argc, char *argv[]) {
    linha=atoi(argv[2]),coluna=atoi(argv[3]);
    int i;
    dboard = malloc (linha * coluna * sizeof(int));
    board = malloc (linha * sizeof(int *));
    for (i=0;i<linha;++i)
        board[i] = &dboard[i*linha];

    dnboard = malloc (linha * coluna * sizeof(int));
    newboard = malloc (linha * sizeof(int *));
    for (i=0;i<linha;++i)
        newboard[i] = &dnboard[i*linha];
    //initialize_board (board);
    read_file (board, argv[1]);

    /* play game of life 100 times */

        play (board);

        /* clear the screen using VT100 escape codes */

//        puts ("\033[H\033[J");
        print (newboard);
}
