#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

const int NUMERO_MAYOR  = 10000;
int cantidadDeHilos = 0;
int cantidadDeGrupos = 0;
int *elementosPorGrupo;
int **matrix;
int intervalo = 0;
pthread_barrier_t mybarrier; 
pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int contador = 0;
int turno;


void *funcion_hilo( void *param );

void intercambiar(int *a, int *b) {
  int temporal = *a;
  *a = *b;
  *b = temporal;
}

void burbuja( int *arreglo, int longitud) {
  for (int x = 0; x < longitud; x++) {
    // Recuerda que el -1 es porque no queremos llegar al final ya que hacemos
    // un indiceActual + 1 y si fuéramos hasta el final, intentaríamos acceder a un valor fuera de los límites
    // del arreglo
    for (int indiceActual = 0; indiceActual < longitud - 1;indiceActual++) {
      int indiceSiguienteElemento = indiceActual + 1;
      // Si el actual es mayor que el que le sigue a la derecha...
      if (arreglo[indiceActual] > arreglo[indiceSiguienteElemento]) {
        // ...intercambiarlos, es decir, mover el actual a la derecha y el de la derecha al actual
        intercambiar(&arreglo[indiceActual], &arreglo[indiceSiguienteElemento]);
      }
    }
  }
}

int main() {

    //abriendo
   FILE *file;
   file = fopen( "input_4.txt", "r" );

   if( file != NULL ){

    do{
        printf( "Ingrese la cantidad de hilos: " );
        scanf( "%d", &cantidadDeHilos );

    }while( NUMERO_MAYOR % cantidadDeHilos != 0 );   

    intervalo = NUMERO_MAYOR / cantidadDeHilos;

    //inicialiizando barrera

    pthread_barrier_init( &mybarrier, NULL, cantidadDeHilos );

    //leyendo la cantidad de grupos
    fscanf( file, "%d", &cantidadDeGrupos );

    //reservando memoria para los elementos por grupo
    elementosPorGrupo = (int *) malloc( sizeof( int )* cantidadDeGrupos );

    //reservando memoria para las filas de la matriz
    matrix = (int **) malloc( sizeof( int * ) * cantidadDeGrupos );

    //recorriendo el archivo y guardando los grupos en la matriz
    //y la cantidad de elementos de los grupos en "elemntosPorGrupo"

    for( int i = 0; i < cantidadDeGrupos; i++) { //recorre los grupos
       
       fscanf( file, "%d", &elementosPorGrupo[ i ] ); // lee la cantidad de elementos que tiene el grupo que se está recorriendo

       matrix[ i ] = (int * ) malloc( sizeof( int ) * elementosPorGrupo[ i ] ); // cuantos enteros se necesita guardar en la primera fila de la matriz
       
       for( int j = 0; j  < elementosPorGrupo[ i ]; j++ ) { // recorre los elementos del grupo que se está recorriendo
            fscanf( file, "%d", &matrix[ i ][ j ] );      
       }
    }


    //creando hilos

    pthread_t hilos[ cantidadDeHilos ]; // guarda los identificadors de los hilos

    for( int i = 0; i < cantidadDeHilos; i++ ) {
        int *indice = (int *) malloc( sizeof( int ) ); //variable que va a guardar la copia del indice
        *indice = i; // copiando el indice

        pthread_create( &hilos[ i ], NULL, funcion_hilo, indice );
    }

    //esperando hilos

    for( int i = 0; i < cantidadDeHilos; i++ ) {
        
        pthread_join( hilos[ i ], NULL );    

    }

    // imprimeindo los elementos 

   for( int i = 0; i < cantidadDeGrupos; i++) { //recorre los grupos
        printf( "Grupo: %d\n\n", i );
    
       for( int j = 0; j  < elementosPorGrupo[ i ]; j++ ) { // recorre los elementos del grupo que se está recorriendo
            printf( "[%d]  %d\n", j , matrix[i][j] );
       }

       printf( "\n\n" );
    }

    free( elementosPorGrupo );
    free( matrix );
    fclose( file );

   } else{

    printf( "Error al abrir el archivo\n" );
    
   }

}

void *funcion_hilo( void *param ) {

    int *indice = (int *)param;
    int almacen[ 100 ];
    int comienzo = *indice * intervalo;
    int fin = comienzo + intervalo;
    int cantidadDeNumerosAsignados = 0;

    for( int i = 0; i < cantidadDeGrupos; i++ ) {
        
        pthread_barrier_wait( &mybarrier );
        cantidadDeNumerosAsignados = 0;

        for( int j = 0; j < elementosPorGrupo[ i ]; j++ ) {

            if( matrix[ i ][ j ] >= comienzo && matrix[ i ][ j ] < fin ) {
                almacen[ cantidadDeNumerosAsignados ] = matrix[ i ][ j ];
                cantidadDeNumerosAsignados++;
            }
        }

        // cada hilo ordena la parte que le corresponde

        burbuja( almacen, cantidadDeNumerosAsignados );
    
        turno = 0;
        contador = 0;
        pthread_barrier_wait( &mybarrier );

        pthread_mutex_lock(&mutex);

        while(turno != *indice) pthread_cond_wait(&cond, &mutex);	

        for( int k = 0 ;  k < cantidadDeNumerosAsignados; k++ ) {
            matrix[ i ][ contador ] = almacen[ k ];
            contador++;
        }
    
        turno++;
        pthread_cond_broadcast(&cond);

        pthread_mutex_unlock(&mutex);
      
    }

}