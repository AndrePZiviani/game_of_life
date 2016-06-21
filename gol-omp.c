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
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

#define TRUE 1
#define FALSE 0
/* set everthing to zero */
int linha,coluna, *m_data, **m, *n_data, **n, **now, **new, *bu, *br, *bd, *bl, *dur, *ddr, *ddl, *dul, seed;
char *mprint;
int *u, *r, *d, *l;
int up,right,down,left,pdur,pddr,pddl,pdul;
char file[50];
FILE *fp;

void initialize_board (int **a, int linha, int coluna) {

#pragma omp parallel private(seed) shared(a)
    {
    int i, j,rmax = RAND_MAX / 10;
    int id = omp_get_thread_num();
    seed+=id;
#pragma omp  for schedule(static,16)
    for (i=1; i<linha-1; ++i) 
        for (j=1; j<coluna-1; ++j){ 
            a[i][j] = rand_r(&seed) < rmax ? 1 : 0;
        }
    }
}

void play (int **old, int **new) {
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
#pragma omp parallel
    {
    int i, j, count;
    int id = omp_get_thread_num();
    int result[9] = {0,0,0,1,0,0,0,0,0};
#pragma omp for schedule(static,32) 

    /* for each cell, apply the rules of Life */

    for (i=1; i<linha-1; ++i) {
        for (j=1; j<coluna-1; ++j) {
            count = old[i-1][j-1] + old[i-1][j] + old[i-1][j+1] +
                    old[i][j-1]   +               old[i][j+1]   +
                    old[i+1][j-1] + old[i+1][j] + old[i+1][j+1];
            result[2] = old[i][j];
            new[i][j] = result[count];
        }
    }
    }

}

/* print the life board */

void print (FILE *fp, int **m, int linha, int coluna) {
    int i, j, x=0;
    mprint[0] = '\0';

    /* for each row */

    for (i=1; i<linha-1; ++i) {

        /* print each column position... */

        for (j=1; j<coluna-1; ++j) {
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

//DEBUG
/*void stdprint (int **m, int linha, int coluna) {
    int i, j, x=0;
    mprint[0] = '\0';


    for (i=0; i<linha; ++i) {


        for (j=0; j<coluna; ++j) {
            if (m[i][j] == 1)
                sprintf(mprint+x, "x");
            else
                sprintf(mprint+x, " ");
            ++x;
            //sprintf (mprint,"%s%c",mprint, m[i][j] ? 'x' : ' ');
        }

        sprintf(mprint+x, "\n");
        ++x;
        //sprintf (mprint,"%s\n",mprint);
    }
    //sprintf (mprint,"%s\n",mprint);
    fprintf (stdout,"%s",mprint);
    fflush(fp);
}*/

void borders (int **m) {
    int i;
#pragma omp parallel
    {
#pragma omp for schedule(static,32) //left and right
    for (i=1; i<linha-1; i++) {
        m[i][0] = m[i][coluna-2];
        m[i][linha-1] = m[i][1];
    }
#pragma omp for schedule(static,32) //upper and bottom
    for (i=1; i<coluna-1; ++i) {
        m[0][i] = m[linha-2][i];
        m[linha-1][i] = m[1][i];
    }

#pragma omp single
    {
    m[0][0] = m[linha-2][coluna-2];
    m[linha-1][0] = m[1][coluna-2];
    m[0][coluna-1] = m[linha-2][1];
    m[linha-1][coluna-1] = m[1][1];
    }
    }
}

double timestamp(void){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return((double)(tp.tv_sec + tp.tv_usec/1000000.0));
}

/* main program */


int main (int argc, char *argv[]) {
    int size,iter,tmp[2],coord[2];

    double inicio, primeiro,init, fim=0, aux;

    inicio = timestamp();

    if (argc != 5) {
        printf ("Passe como parametro as dimensoes da matriz de cada processo em linhas e colunas e o layout!\n\
                %s [linhas] [colunas] [iteracoes] [semente]\n", argv[0]);
        return 1;
    }

    linha = atoi(argv[1]);
    coluna = atoi(argv[2]);
    iter = atoi(argv[3]);

    seed = atoi(argv[4]);
    //srand(atoi(argv[4]));

    // somei 2 para colocar as bordas direto na matriz
    linha += 2;
    coluna += 2;

    m_data = malloc (linha * coluna * sizeof(int));
    m = malloc (linha * sizeof(int *));
    for (int i=0;i<linha;++i)
        m[i] = &m_data[i*coluna];

    n_data = malloc (linha * coluna * sizeof(int));
    n = malloc (linha * sizeof(int *));
    for (int i=0;i<linha;++i)
        n[i] = &n_data[i*coluna];

    mprint = malloc ((linha * (coluna + 2) * sizeof(int)) + 2);

    now = m;
    new = n;
    int **temp;

    int num_threads;
//#pragma omp parallel shared(result,dresult)
//    {
    // inicializa o vetor de "hash" para decidir o proximo estado da celula
//#pragma omp single
//    {
        //num_threads = omp_get_num_threads();
        //printf("%d\n",num_threads);
        //result = malloc (num_threads*sizeof(int*));
        //dresult = malloc (9*num_threads*sizeof(int));
    //int id = omp_get_thread_num();
    //int j;
    //for (int i=0; i<num_threads; ++i)
    //    result[i] = &dresult[i*9];
    //for (j=0; j<num_threads; ++j) {
    //    for (int i=0; i<9; ++i)
    //        result[id][i] = 0;
    //    result[id][3] = 1;
    //}
    //}
    //}

    primeiro = timestamp() - inicio;

    init = timestamp();
    initialize_board (m,linha,coluna);
    init = timestamp() - init;


    int i=0;
        for (i=0; i<iter; i++) {
#ifdef DEBUG
            if (iter > 5) {
                printf("Erro! Numero de iteracoes maior que 5, com isso iria gerar muito IO. Abortando!\n");
                return 1;
            }
            
            sprintf (file, "%s-%d.out",argv[0],i);
            fp = fopen (file,"w");
            print (fp,now,linha,coluna);
            fclose(fp);
#endif
            
            aux = timestamp();

        /*border_left(now);
        border_right(now);
        border_upper(now); //ja faz diagonal
        border_bottom(now); //ja faz diagonal*/
            borders (now);
            play (now,new);

            temp = now;
            now = new;
            new = temp;

            fim += timestamp() - aux;
        }

    sprintf(file, "%s.stat",argv[0]);
    fp = fopen(file,"w");
    fprintf (fp,"Tempo de mallocs: %e\nTempo de Inicializacao: %e\nTempo Medio por Iteracao: %e\nTempo Total: %e\n",primeiro,init,fim/iter,timestamp()-inicio);
    fflush(fp);
    fclose(fp);

#ifdef DEBUG
    sprintf (file, "%s-%d.out",argv[0],i);
#else
    sprintf (file, "%s-0.out",argv[0]);
#endif
    fp = fopen (file,"w");
    print (fp,now,linha,coluna);
    fflush(fp);
    fclose(fp);

    free(m_data);
    free(m);
    free(n_data);
    free(n);

}
