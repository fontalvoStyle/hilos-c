#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

int **matrizA;
int **matrizB;
int **matrizC;
int taman = 0;

void *funcion_hilo( void *param );


int main() {

    printf( "\nIngresa el tamaño de las matrices: " );
    scanf( "%d", &taman );

    //espacio en memoria para las filas que va a guardar cada matriz

    matrizA = ( int ** ) malloc( sizeof( int *) * taman );
    matrizB = ( int ** ) malloc( sizeof( int *) * taman );
    matrizC = ( int ** ) malloc( sizeof( int *) * taman );

    // dando espacio en memoria a matriz A y matriz B y llenandolas con datos

    for( int i = 0; i < taman; i++ ) {

        matrizA[ i ] = (int *) malloc( sizeof( int ) * taman );
        matrizB[ i ] = (int *) malloc( sizeof( int ) * taman );
        matrizC[ i ] = (int *) malloc( sizeof( int ) * taman );

        for( int j = 0; j < taman; j++ ) {
            matrizA[ i ][ j ] = rand() % 9 + 1; 
            matrizB[ i ][ j ] = rand() % 9 + 1; 
        }

    }

    //imprimiendo matrices

    printf( "\nImprimiendo matriz A\n" );

    for( int i = 0; i < taman; i++ ) {

        for( int j = 0; j < taman; j++ ) {

            printf( "[ %d ]", matrizA[ i ][ j ] );
            
        }
        printf( "\n" );
    }

    printf( "\nImprimiendo matriz B\n" );

    for( int i = 0; i < taman; i++ ) {

        for( int j = 0; j < taman; j++ ) {

            printf( "[ %d ]", matrizB[ i ][ j ] );
            
        }
        printf( "\n" );
    }

    int numeroDeHilos = taman / 2;

    pthread_t hilos[ numeroDeHilos ]; 

    for( int i = 0; i < numeroDeHilos ; i++ ) {
        
        int *indice = (int * ) malloc( sizeof( int ) );
        *indice = i;
        pthread_create( &hilos[ i ], NULL, funcion_hilo, indice );
    }
    
    for( int i = 0; i < numeroDeHilos ; i++ ) {
        pthread_join( hilos[ i ], NULL );
    }
    
    printf( "\nImprimiendo matriz C\n" );

    for( int i = 0; i < taman; i++ ) {

        for( int j = 0; j < taman; j++ ) {

            printf( "[ %d ]", matrizC[ i ][ j ] );
            
        }
        printf( "\n" );
    }

    free( matrizA );
    free( matrizB );
    free( matrizC );


}

void *funcion_hilo( void *param ) {

    int *indice = (int *)param;
    int inicio = *indice;
    int final = taman - 1 - *indice;
   

    for( int i = 0;  i < taman; i++ ) {

        for( int j = 0; j < taman; j++ ){

            if( ( ( i == inicio || i == final ) && j >= inicio && j <= final ) || ( ( j == inicio || j == final ) && i >= inicio && i <= final )  ) {
                
                matrizC[ i ][ j ] = 0;

                for( int k = 0; k < taman; k++ ) {
                    matrizC[ i ][ j ] += matrizA[ i ][ k ] * matrizB[ k ][ j ];
                }
            }
            
        }
    }
}