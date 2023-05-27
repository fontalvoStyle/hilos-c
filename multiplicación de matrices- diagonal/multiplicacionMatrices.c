#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int **matrizA;
int **matrizB;
int **matrizC;

void *funcion_hilo( void *param );

int taman;

int main() {

    printf( "\nPor favor ingresa el tamaño de las matrices: " );
    scanf( "%d", &taman );

    //llenamos la matriz A y la matriz B con datos aleatorios

        matrizA = (int **) malloc( sizeof( int * ) * taman );
        matrizB = (int **) malloc( sizeof( int *)  * taman );
        matrizC = (int **) malloc( sizeof( int *)  * taman );

    for( int i = 0 ; i < taman; i++ ) {
        matrizA[ i ] = (int *)malloc( sizeof( int ) * taman );
        matrizB[ i ] = (int *)malloc( sizeof( int ) * taman );
        matrizC[ i ] = (int *)malloc( sizeof( int ) * taman );
        for( int j = 0; j < taman; j++ ){
            matrizA[ i ][ j ] = rand() % 9 + 1;
            matrizB[ i ][ j ] = rand() % 9 + 1;
        }
    }

    //Imprimiendo las matrices

     printf( "\nImprimiendo matriz A\n" );

    for( int i = 0 ; i < taman; i++ ) {
       
        for( int j = 0; j < taman; j++ ){
            printf( "[ %d ]", matrizA[i ][ j ] );
        }
        printf( "\n" );
    }

    printf( "\nImprimiendo matriz B\n" );

    for( int i = 0 ; i < taman; i++ ) {
       
        for( int j = 0; j < taman; j++ ){
            printf( "[ %d ]", matrizB[i ][ j ] );
        }
        printf( "\n" );
    }

    int cantidadDeHilos = 3;

    pthread_t hilos[ cantidadDeHilos ];

    //creamos hilos

    for( int i = 0; i < cantidadDeHilos; i++ ) {
        int *indice = (int * ) malloc( sizeof( int ) );
        *indice = i;

        pthread_create( &hilos[ i ], NULL,funcion_hilo, indice );
    }

    //esperamos hilos

    for( int i = 0; i < cantidadDeHilos; i++ ) {
        pthread_join( hilos[ i ], NULL );
    }

    printf( "\nImprimiendo matriz C\n" );

    for( int i = 0 ; i < taman; i++ ) {
       
        for( int j = 0; j < taman; j++ ){
            printf( "[ %d ]", matrizC[i ][ j ] );
        }
        printf( "\n" );
    }

    free( matrizA );
    free( matrizB );
    free( matrizC );



}

void *funcion_hilo( void *param ){

    int *indice = (int*) param;

    for( int i = 0; i < taman; i++ ) {

        for( int j = 0; j < taman; j++ ) {

            if( (*indice == 0 && i > j) || ( *indice == 1 && i == j ) || ( *indice == 2 && j > i )  ) {
                
                matrizC[ i ][ j ] = 0;

                for( int k = 0; k < taman; k++ ) {
                    matrizC[ i ][ j ] += matrizA[ i ][ k ] * matrizB[ k ][ j ];
                }

            }

            
        }
        
    }

}