#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void *funcion_hilo( void *param );

int **matriz;
int *vector;
int *vectorResultante;
int filas = 0;
int columnas = 0;
int *filasAsignadas;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier;

int main(){

    printf( "\nPor favor ingresa el número de filas: " );
    scanf( "%d", &filas );

    printf( "\nPor favor ingresa el número de columnas: " );
    scanf( "%d", &columnas );


    matriz = (int **) malloc( sizeof( int *) * filas );

    vector = (int *) malloc( sizeof( int ) * columnas );

    filasAsignadas = (int *) malloc( sizeof( int ) * filas );

    vectorResultante = (int *) malloc( sizeof( int ) * filas );
       
    // llenando el vector de filas asignadas con -1 indicando que la fila en dicha posición no ha sido tomada por ningún hilo

    for( int i = 0; i < filas; i++ ) {
        filasAsignadas[ i ] = -1;
    }

    //llenando el vector con datos aleatorios
    for( int i = 0; i < columnas; i++ ) {
        vector[ i ] = rand() % 9 + 1;
    }

    printf( "\nImprimiendo vector\n\n" );

    for( int i = 0; i < columnas; i++ ) {
        printf( "[ %d ]",vector[ i ] );
    }

    //llenando la matriz con datos aleatorios

    for( int i = 0; i < filas; i++ ) {
        matriz[ i ] = ( int * ) malloc( sizeof( int ) * columnas );

        for( int j = 0; j < columnas; j++ ) {
            matriz[ i ][ j ] = rand() % 9 + 1;
        }
    }

    printf( "\n\nImprimiendo matriz\n\n" );

    for( int i = 0; i < filas; i++ ) {

        for( int j = 0; j < columnas; j++ ) {
            printf( "[ %d ]", matriz[ i ][ j ] );
        }
        printf( "\n");
    }

    int cantidadDeHilos = 0; //almacena la cantidad de hilos que ingresa el usuario

    do{
        printf( "\nIngresa la cantidad de hilos que harán la multiplicación: " );
        scanf( "%d", &cantidadDeHilos );
    }while( cantidadDeHilos > filas || cantidadDeHilos < 0 );
    

    pthread_t hilos[ cantidadDeHilos ]; // array que contiene los identificadores de los hilos
    pthread_barrier_init( &barrier, NULL, cantidadDeHilos );

    for( int i = 0; i < cantidadDeHilos; i++ ) {
        int *indice = (int *) malloc( sizeof( int ) );
        *indice = i;  //creando copia del indice

        pthread_create( &hilos[ i ], NULL, funcion_hilo, indice ); 

    }

    //esperando hilos

    for( int i = 0; i < cantidadDeHilos; i++ ) {
        pthread_join( hilos[ i ], NULL );

    }

    printf( "\nImprimiendo vector resultante\n\n");

    for( int i = 0; i < filas; i++ ) {

        printf( "[ %d ]",vectorResultante[ i ] );
    }
    printf( "\n" );


    free( matriz );
    free( vector );
    free( filasAsignadas );

}

void *funcion_hilo( void *param ) {

    int *indice = (int *) param;
    int filaAsignada = *indice;
    filasAsignadas[ * indice ] = 1;

    pthread_barrier_wait( &barrier );

    do{

        for( int j = 0; j < columnas; j++ ){
            vectorResultante[ filaAsignada ] += vector[ j ] * matriz[ filaAsignada ][ j ];
        }
            
        filaAsignada = -1;
        pthread_mutex_lock( &mutex );

            for( int k = 0; k < filas; k++ ){
                if( filasAsignadas [ k ] == -1 ){
                    filaAsignada = k;
                    filasAsignadas[ k ] = 1;
                    break;
                }
            }

        pthread_mutex_unlock( &mutex );

    }while( filaAsignada != -1 );


}