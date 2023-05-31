#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *funcion_hilo( void *param );

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_barrier_t barrier;

int turno = 0;

int main(){

    int cantidadDeHilos = 0;

    printf( "Ingresa la cantidad de hilos que deseas ejecutar: " );
    scanf( "%d", &cantidadDeHilos );

    pthread_t identificadoresHilos[ cantidadDeHilos ];

    for( int i = 0; i < cantidadDeHilos; i++ ) {
        int *copiaIndice = (int *) malloc( sizeof( int ) );
        *copiaIndice = i;

        pthread_create( &identificadoresHilos[ i ], NULL, funcion_hilo, copiaIndice );
    }

    for( int i = 0; i < cantidadDeHilos; i++ ) {
       pthread_join( identificadoresHilos[ i ], NULL );
    }

}

void *funcion_hilo( void *param ) {

    int *indice = (int *)param;

    pthread_mutex_lock( &mutex );

    while( turno != *indice ){
        pthread_cond_wait( &cond, &mutex );
    }

   printf( "Hola desde el hilo %d\n", *indice );
   turno++;  
   pthread_cond_broadcast( &cond );
   pthread_mutex_unlock( &mutex );

}