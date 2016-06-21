#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0
/* set everthing to zero */
int row, column, *m_data, **m, *n_data, **n, **current, **new, *bu, *br, *bd,
    *bl, *dur, *ddr, *ddl, *dul, *result;
char *mprint;
int *u, *r, *d, *l;
int up, right, down, left, pdur, pddr, pddl, pdul;
char file[50];
FILE *fp;

void initialize_board (int **a, int row, int column) {
    int i, j;

    for (i=1; i<row-1; ++i) 
        for (j=1; j<column-1; ++j){ 
            a[i][j] = rand() < RAND_MAX / 10 ? 1 : 0;
        }
}

void play (int **old, int **new) {
/*
    (copied this from some web page, hence the English spellings...)

    1.STASIS : If, for a given cell, the number of on neighbours is 
    exactly two, the cell maintains its status quo into the next 
    generation. If the cell is on, it stays on, if it is off, it stays off.

    2.GROWTH : If the number of on neighbours is exactly three, the cell 
    will be on in the next generation. This is regardless of the cell's
    current state.

    3.DEATH : If the number of on neighbours is 0, 1, 4-8, the cell will 
    be off in the next generation.
*/
    int i, j, count;

    /* for each cell, apply the rules of Life */

    for (i=1; i<row-1; ++i) {
        for (j=1; j<column-1; ++j) {
            count = old[i-1][j-1] + old[i-1][j] + old[i-1][j+1] +
                    old[i][j-1]   +               old[i][j+1]   +
                    old[i+1][j-1] + old[i+1][j] + old[i+1][j+1];
            result[2] = old[i][j];
            new[i][j] = result[count];
        }
    }

}

/* print the life board */

void print (FILE *fp, int **m, int row, int column) {
    int i, j, x=0;
    mprint[0] = '\0';

    /* for each column */

    for (i=1; i<row-1; ++i) {

        /* print each column position */

        for (j=1; j<column-1; ++j) {
            if (m[i][j] == 1)
                sprintf(mprint+x, "x");
            else
                sprintf(mprint+x, " ");
            ++x;
            //sprintf (mprint,"%s%c",mprint, m[i][j] ? 'x' : ' ');
        }

        /* followed by a carriage return */
        sprintf(mprint+x, "\n");
        ++x;
        //sprintf (mprint,"%s\n",mprint);
    }
    //sprintf (mprint,"%s\n",mprint);
    fprintf (fp,"%s",mprint);
    fflush(fp);
}

void borders (int **m) {
    //copy columns
    for (int i=1; i<row-1; ++i) {
        m[i][column-1] = m[i][1];
        m[i][0] = m[i][column-2];
    }

    //copy rows
    for (int i=1; i<column-1; ++i) {
        m[0][i] = m[row-2][i];
        m[row-1][i] = m[1][i];
    }

    //copy corners
    m[0][0] = m[row-2][column-2];
    m[0][column-1] = m[row-2][1];
    m[row-1][0] = m[1][column-2];
    m[row-1][column-1] = m[1][1];


}

double timestamp(void){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return((double)(tp.tv_sec + tp.tv_usec/1000000.0));
}

/* main program */

int main (int argc, char *argv[]) {
    int iter,seed;

    double start, first, end=0, aux, total, serial;
    char* outfile;

    start = timestamp();

    if (argc < 5 || argc > 6) {
        printf ("Usage:\n\
                %s <linhas> <colunas> <iteracoes> <output_file> [semente]\n", argv[0]);
        return 1;
    }

    row = atoi(argv[1]);
    column = atoi(argv[2]);
    iter = atoi(argv[3]);
    outfile = argv[4];
    seed = argv[5]? atoi(argv[5]) : (int) timestamp();

    srand(seed);

    // add 2 for borders copy
    row += 2;
    column += 2;

    m_data = malloc(row * column * sizeof(int));
    if (!m_data) {
        printf("Erro de memória, talvez tentar uma entrada menor?");
        exit(1);
    }
    m = malloc (row * sizeof(int *));
    if (!m) {
        printf("Erro de memória, talvez tentar uma entrada menor?");
        exit(1);
    }
    for (int i=0;i<row;++i)
        m[i] = &m_data[i*column];

    n_data = malloc(row * column * sizeof(int));
    if (!n_data) {
        printf("Erro de memória, talvez tentar uma entrada menor?");
        exit(1);
    }
    n = malloc (row * sizeof(int *));
    if (!n) {
        printf("Erro de memória, talvez tentar uma entrada menor?");
        exit(1);
    }
    for (int i=0;i<row;++i)
        n[i] = &n_data[i*column];

    mprint = malloc((row * (column + 2) * sizeof(int)) + 2);

    current = m;
    new = n;
    int **temp;

    // initialize "hash" vector for next state
    result = malloc (9*sizeof(int));
    for (int i=0; i<9; ++i)
        result[i] = 0;
    result[3] = 1;

    initialize_board(m, row, column);

    first = timestamp() - start;
    serial = first;

    int i = 0;
    for ( i=0 ; i<iter ; i++) {
#ifdef DEBUG
        if (iter > 5) {
            printf("Erro! Numero de iteracoes maior que 5 gera muito IO.\
                   Abortando!\n");
            return 1;
        }
        
        sprintf(file, "%s-%d.out",argv[0],i);
        fp = fopen (file,"w");
        print(fp, current, row, column);
        fclose(fp);
#endif
        
        aux = timestamp();

        borders(current);
        serial += timestamp() - aux; //serial time is init time + border copy

        play (current,new);

        temp = current;
        current = new;
        new = temp;

        end += timestamp() - aux;
    }

#ifdef DEBUG
    sprintf (file, "%s-%d.out",argv[0],i);
#else
    sprintf (file, "%s", outfile);
#endif
    fp = fopen (file,"w");
    print (fp, current, row, column);
    fflush(fp);
    fclose(fp);

    free(m_data);
    free(m);
    free(n_data);
    free(n);

    total = timestamp()-start;
    
    fp = fopen("stats.out","w");
    fprintf (fp,"------------------------------------------\n"
                "Tempo de Inicializacao: %e\n"
                "Tempo Medio por Iteracao: %e\n"
                "Tempo Parte Serial: %e\n"
                "Tempo Total: %e\n"
                "Fracao Serial %e\n"
                "------------------------------------------\n",
                first, end/iter, serial, total, serial/total);
    fflush(fp);
    fclose(fp);

}
