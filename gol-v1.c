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
#include <mpi.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0
/* set everthing to zero */
MPI_Comm cart;
int linha,coluna, *m_data, **m, *n_data, **n, **now, **new, *bu, *br, *bd, *bl, *dur, *ddr, *ddl, *dul, eu;
int *u, *r, *d, *l;
int up,right,down,left,pdur,pddr,pddl,pdul;
char file[50];
FILE *fp;

void initialize_board (int **a, int linha, int coluna) {
    int i, j;

    for (i=0; i<linha; ++i) 
        for (j=0; j<coluna; ++j){ 
            a[i][j] = rand() < RAND_MAX / 10 ? 1 : 0;
        }
}

/* return the number of on cells adjacent to the i,j cell */

int adjacent_to (int **m,int i, int j) {
    int  count;

    count = 0;

    /* go around the cell */

    if (i>0 && i<linha-1 && j>0 && j<coluna-1) 
        // estou no meio da matriz
        count = m[i-1][j-1] + m[i-1][j] + m[i-1][j+1] +
                m[i][j-1]   +             m[i][j+1]   +
                m[i+1][j-1] + m[i+1][j] + m[i+1][j+1];
    else if (i==0)
        // pega do bu
        if (j==0)
            // estou na 0,0 tem que pegar da bl bu e diagonal bul
            count = *dul         + bu[0]   + bu[1]   +
                    bl[0]        +           m[0][1] +
                    bl[1]        + m[1][0] + m[1][1];
        else if (j==coluna-1)
            // estou na 0,coluna tem que pegar da br bu e diagonal bur
            count = bu[j-1]   + bu[j]   + *dur           +
                    m[0][j-1] +           br[0]         +
                    m[1][j-1] + m[1][j] + br[1];
        else
            // estou na 0,[1~coluna) tem que pegar so da bu
            count = bu[j-1]   + bu[j]   + bu[j+1]   +
                    m[0][j-1] +           m[0][j+1] +
                    m[1][j-1] + m[1][j] + m[1][j+1];
    else if (i==linha-1)
        // pega do bd
        if (j==0)
            // estou na linha,0 tem que pegar da bl bd e diagonal bdl
            count = bl[i-1]    + m[i-1][0]   + m[i-1][1] +
                    bl[i]      +               m[i][1]   +
                    *ddl        + bd[0]       + bd[1];
        else if (j==coluna-1)
            // estou na linha,coluna tem que pegar da br bd e diagonal bdr
            count = m[i-1][j-1] + m[i-1][j] + br[i-1] +
                    m[i][j-1]   +             br[i]   +
                    bd[j-1]     + bd[j]     + *ddr;
        else
            // estou na linha,[1~coluna) tem que pegar so da bd
            count = m[i-1][j-1] + m[i-1][j] + m[i-1][j+1] +
                    m[i][j-1]   +             m[i][j+1]   +
                    bd[j-1]     + bd[j]     + bd[j+1];
    else if (j==0)
        //pega do bl
        // estou na [1~linha),0 tem que pegar so da bu
        count = bl[i-1] + m[i-1][0] + m[i-1][1] +
                bl[i]   +             m[i][1]   +
                bl[i+1] + m[i+1][0] + m[i+1][1];
    else if (j==coluna-1)
        // pega do br
        // esotou na [1~linha),coluna tem que pegar so da br
        count = m[i-1][j-1] + m[i-1][j] + br[i-1] +
                m[i][j-1]   +             br[i]   +
                m[i+1][j-1] + m[i+1][j] + br[i+1];

    return count;
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
    int i, j, a;

    /* for each cell, apply the rules of Life */

    for (i=0; i<linha; ++i)
        for (j=0; j<coluna; ++j) {
            a = adjacent_to (old,i,j);
            if (a == 2) new[i][j] = old[i][j];
            if (a == 3) new[i][j] = 1;
            if (a < 2) new[i][j] = 0;
            if (a > 3) new[i][j] = 0;
        }

}

/* print the life board */

void print (FILE *fp, int **m, int linha, int coluna) {
    int i, j;

    /* for each row */

    for (i=0; i<linha; ++i) {

        /* print each column position... */

        //fprintf (fp,"#");
        for (j=0; j<coluna; ++j) {
            fprintf (fp,"%c", m[i][j] ? 'x' : ' ');
            fflush(fp);
        }

        /* followed by a carriage return */
        //fprintf (fp,"#");
        fprintf (fp,"\n");
    }
    fflush(fp);
}

void borders (int **m) {
    MPI_Request request;
    MPI_Status status;
    u = m[0];
    d = m[linha-1];

    for (int i=0; i<linha; ++i)
        l[i] = m[i][0];

    for (int i=0; i<linha; ++i)
        r[i] = m[i][coluna-1];

    MPI_Isend (u,coluna,MPI_INT,up,0,cart,&request);
    MPI_Isend (r,linha,MPI_INT,right,0,cart,&request);
    MPI_Isend (d,coluna,MPI_INT,down,0,cart,&request);
    MPI_Isend (l,linha,MPI_INT,left,0,cart,&request);

    MPI_Isend (&m[0][coluna-1],1,MPI_INT,pdur,0,cart,&request);
    MPI_Isend (&m[linha-1][coluna-1],1,MPI_INT,pddr,0,cart,&request);
    MPI_Isend (&m[linha-1][0],1,MPI_INT,pddl,0,cart,&request);
    MPI_Isend (&m[0][0],1,MPI_INT,pdul,0,cart,&request);

    MPI_Recv (bu,coluna,MPI_INT,up,0,cart,&status);
    MPI_Recv (br,linha,MPI_INT,right,0,cart,&status);
    MPI_Recv (bd,coluna,MPI_INT,down,0,cart,&status);
    MPI_Recv (bl,linha,MPI_INT,left,0,cart,&status);

    MPI_Recv (dur,1,MPI_INT,pdur,0,cart,&status);
    MPI_Recv (ddr,1,MPI_INT,pddr,0,cart,&status);
    MPI_Recv (ddl,1,MPI_INT,pddl,0,cart,&status);
    MPI_Recv (dul,1,MPI_INT,pdul,0,cart,&status);

    MPI_Barrier (cart);

}

double timestamp(void){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return((double)(tp.tv_sec + tp.tv_usec/1000000.0));
}

/* main program */


int main (int argc, char *argv[]) {
    int dim[2],ciclico[2],size,iter,tmp[2],coord[2];

    double inicio, primeiro, fim=0, aux;

    inicio = timestamp();

    if (argc != 6) {
        printf ("Passe como parametro as dimensoes da matriz de cada processo em linhas e colunas e o layout!\n\
                %s [linhas] [colunas] [x] [y] [iteracoes]\n", argv[0]);
        return 1;
    }

    linha = atoi(argv[1]);
    coluna = atoi(argv[2]);
    dim[1] = atoi(argv[3]); //sim, primeiro coluna depois linha no vetor
    dim[0] = atoi(argv[4]);
    iter = atoi(argv[5]);
    ciclico[0]=1;
    ciclico[1]=1;


    MPI_Init(&argc, &argv);
    MPI_Cart_create(MPI_COMM_WORLD,2,dim,ciclico,TRUE,&cart);
    MPI_Comm_size(cart, &size);


    MPI_Comm_rank(cart,&eu);
    srand(time(NULL)+eu);

    m_data = malloc (linha * coluna * sizeof(int));
    m = malloc (linha * sizeof(int *));
    for (int i=0;i<linha;++i)
        m[i] = &m_data[i*linha]; 

    n_data = malloc (linha * coluna * sizeof(int));
    n = malloc (linha * sizeof(int *));
    for (int i=0;i<linha;++i)
        n[i] = &n_data[i*linha]; 

    now = m;
    new = n;
    int **temp;

    bu = malloc (coluna * sizeof(int));
    br = malloc (linha * sizeof(int));
    bd = malloc (coluna * sizeof(int));
    bl = malloc (linha * sizeof(int));

    dur = malloc (sizeof(int));
    ddr = malloc (sizeof(int));
    ddl = malloc (sizeof(int));
    dul = malloc (sizeof(int));

    // aloca os vetores para mandar no borders
    l = malloc (linha * sizeof(int));
    r = malloc (linha * sizeof(int));

    // pega meus vizinhos
    MPI_Cart_coords(cart,eu,2,coord);

    MPI_Cart_shift(cart,0,1,&left,&right);
    MPI_Cart_shift(cart,1,1,&up,&down);

    tmp[1] = coord[1] - 1;
    tmp[0] = coord[0] + 1;
    MPI_Cart_rank(cart,tmp,&pdur);

    tmp[1] = coord[1] + 1;
    tmp[0] = coord[0] + 1;
    MPI_Cart_rank(cart,tmp,&pddr);

    tmp[1] = coord[1] + 1;
    tmp[0] = coord[0] - 1;
    MPI_Cart_rank(cart,tmp,&pddl);

    tmp[1] = coord[1] - 1;
    tmp[0] = coord[0] - 1;
    MPI_Cart_rank(cart,tmp,&pdul);


    initialize_board (m,linha,coluna);

    primeiro = timestamp() - inicio;
    int i;
    for (i=0; i<iter; i++) {
//        sprintf (file, "/home/apz11/gol/%d-%d.out",eu,i);
//        fp = fopen (file,"w");

        //fprintf(fp,"############################ %d #######################\n",eu);
//        print (fp,now,linha,coluna);
        //fprintf(fp,"############################ %d #######################\n",eu);

        aux = timestamp();

        borders (now);
        play (now,new);

        //puts ("\033[H\033[J");
        temp = now;
        now = new;
        new = temp;

        fim += timestamp() - aux;
    }
        sprintf (file, "/home/apz11/gol/%d-%d.out",eu,i);
        fp = fopen (file,"w");
        print (fp,now,linha,coluna);
        fflush(fp);
        fclose(fp);
    free(m_data);
    free(m);
    free(n_data);
    free(n);
    MPI_Finalize();
    fp = fopen("statistics-v1.out","w");
    fprintf (fp,"Tempo de inicializacao: %1.5f\nTempo Medio por Iteracao: %1.5f\nTempo Total: %1.5f\n",primeiro,fim/iter,timestamp()-inicio);
    fflush(fp);
    fclose(fp);
}
