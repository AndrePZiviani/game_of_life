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

/* FIXME
 *
 * Usei o "stepper" para calcular o resultado do proximo quadro baseado no atual
 * mas por algum motivo o meu quadro esta deslocado de uma posicao para esquerda
 * (ou o stepper esta deslocado uma posicao para a direito)
 *
 * Verificar se nao tem alguma coisa hardcoded
 */

/* Estou rodando com:
 * mpirun -n 9 ./gol 10 10 3 3 10 && ./merge.sh
 * 9 processos
 * layout dos processos 3x3
 * matrix 10x10
 * 10 iteracoes
 */

/* grava arquivo com
 * MPI_File f;
 *  MPI_File_open(MPI_COMM_SELF, arq,
 *                                  MPI_MODE_WRONLY | MPI_MODE_CREATE | MPI_MODE_UNIQUE_OPEN,
 *                                                                  MPI_INFO_NULL, &f);
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <mpi.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0
/* set everthing to zero */
MPI_Comm cart;
int linha,coluna, *m_data, **m, *n_data, **n, **now, **new, *bu, *br, *bd, *bl, *dur, *ddr, *ddl, *dul, eu;
char *mprint;
int *u, *r, *d, *l;
int up,right,down,left,pdur,pddr,pddl,pdul;
char file[50];
FILE *fp;

void initialize_board (int **a, int linha, int coluna) {
    int i, j;

    for (i=1; i<linha-1; ++i) 
        for (j=1; j<coluna-1; ++j){ 
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
    will be on in the next generation. This is regardless of the cell's         current state.

    3.DEATH : If the number of on neighbours is 0, 1, 4-8, the cell will 
    be off in the next generation.
*/
    int i, j, count;

    /* for each cell, apply the rules of Life */

    for (i=1; i<linha-1; ++i)
        for (j=1; j<coluna-1; ++j) {
            count = old[i-1][j-1] + old[i-1][j] + old[i-1][j+1] +
                    old[i][j-1]   +               old[i][j+1]   +
                    old[i+1][j-1] + old[i+1][j] + old[i+1][j+1];
            if (count == 2) new[i][j] = old[i][j];
            if (count == 3) new[i][j] = 1;
            if (count < 2) new[i][j] = 0;
            if (count > 3) new[i][j] = 0;
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

void borders (int **m) {
    MPI_Request request;
    MPI_Status status;

//    for (int i=1; i<coluna-1; ++i)
//        u[i-1] = m[1][i];
    for (int i=1; i<linha-1; ++i)
        r[i-1] = m[i][coluna-2];
//    for (int i=1; i<coluna-1; ++i)
//        d[i-1] = m[linha-2][i];
    for (int i=1; i<linha-1; ++i)
        l[i-1] = m[i][1];

    MPI_Isend (m[1],coluna-2,MPI_INT,up,0,cart,&request);
    MPI_Isend (r,linha-2,MPI_INT,right,0,cart,&request);
    MPI_Isend (m[linha-2],coluna-2,MPI_INT,down,0,cart,&request);
    MPI_Isend (l,linha-2,MPI_INT,left,0,cart,&request);

    MPI_Isend (&m[1][coluna-2],1,MPI_INT,pdur,0,cart,&request);
    MPI_Isend (&m[linha-2][coluna-2],1,MPI_INT,pddr,0,cart,&request);
    MPI_Isend (&m[linha-2][1],1,MPI_INT,pddl,0,cart,&request);
    MPI_Isend (&m[1][1],1,MPI_INT,pdul,0,cart,&request);

    MPI_Recv (m[0],coluna-2,MPI_INT,up,0,cart,&status);
//    MPI_Recv (u,coluna-2,MPI_INT,up,0,cart,&status);
    MPI_Recv (r,linha-2,MPI_INT,right,0,cart,&status);
    MPI_Recv (m[linha-1],coluna-2,MPI_INT,down,0,cart,&status);
//    MPI_Recv (d,coluna-2,MPI_INT,down,0,cart,&status);
    MPI_Recv (l,linha-2,MPI_INT,left,0,cart,&status);

//    for (int i=1; i<linha-1; ++i)
//        m[0][i] = u[i-1];
    for (int i=1; i<linha-1; ++i)
        m[i][coluna-1] = r[i-1];
//    for (int i=1; i<linha-1; ++i)
//        m[linha-1][i] = d[i-1];
    for (int i=1; i<linha-1; ++i)
        m[i][0] = l[i-1];

    MPI_Recv (&m[0][coluna-1],1,MPI_INT,pdur,0,cart,&status);
    MPI_Recv (&m[linha-1][coluna-1],1,MPI_INT,pddr,0,cart,&status);
    MPI_Recv (&m[linha-1][0],1,MPI_INT,pddl,0,cart,&status);
    MPI_Recv (&m[0][0],1,MPI_INT,pdul,0,cart,&status);

    //MPI_Barrier (cart);

}

double timestamp(void){
    struct timeval tp;
    gettimeofday(&tp,NULL);
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

    // somei 2 para colocar as bordas direto na matriz
    linha += 2;
    coluna += 2;

    m_data = malloc (linha * coluna * sizeof(int));
    m = malloc (linha * sizeof(int *));
    for (int i=0;i<linha;++i)
        m[i] = &m_data[i*linha]; 

    n_data = malloc (linha * coluna * sizeof(int));
    n = malloc (linha * sizeof(int *));
    for (int i=0;i<linha;++i)
        n[i] = &n_data[i*linha]; 

    mprint = malloc ((linha * (coluna + 2) * sizeof(int)) + 2);

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
    l = malloc ((linha-2) * sizeof(int));
    r = malloc ((linha-2) * sizeof(int));
    u = malloc ((coluna-2) * sizeof(int));
    d = malloc ((coluna-2) * sizeof(int));

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
#ifdef DEBUG
        if (iter > 5) {
            printf("Erro! Numero de iteracoes maior que 5, com isso iria gerar muito IO. Abortando!\n");
            return 1;
        }
        sprintf (file, "%s-%d-%d.out",argv[0],eu,i);
        fp = fopen (file,"w");
        print (fp,now,linha,coluna);
        fclose(fp);
#endif

        aux = timestamp();

        borders (now);
        play (now,new);

        temp = now;
        now = new;
        new = temp;

        fim += timestamp() - aux;
    }

#ifdef DEBUG
    sprintf (file, "%s-%d-%d.out",argv[0],eu,i);
#else
    sprintf (file, "%s-%d-0.out",argv[0],eu);
#endif
    fp = fopen (file,"w");
    print (fp,now,linha,coluna);
    fflush(fp);
    fclose(fp);

    free(m_data);
    free(m);
    free(n_data);
    free(n);
    free(bu);
    free(br);
    free(bd);
    free(bl);
    free(dur);
    free(ddr);
    free(ddl);
    free(dul);
    free(l);
    free(r);
    free(u);
    free(d);

    MPI_Finalize();
    fp = fopen("statistics-v2.out","w");
    fprintf (fp,"Tempo de inicializacao: %1.5f\nTempo Medio por Iteracao: %1.5f\nTempo Total: %1.5f\n",primeiro,fim/iter,timestamp()-inicio);
    fflush(fp);
    fclose(fp);
}
