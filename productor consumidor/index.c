#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void *productor( void *param );
void *consumidor( void *param );
void *controlador( void *param );
int control = 1;

int *productos;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int posInsertOrDelete = 0;

int main() {

    pthread_t hilos[ 2 ];

    productos = (int *) malloc( sizeof( int )* 20 );

    pthread_create( &hilos[ 0 ], NULL, productor, NULL );
    pthread_create( &hilos[ 1 ], NULL, consumidor, NULL );


    for( int i = 0; i < 2; i++ ){  
        pthread_join( hilos[ i ], NULL );
    }

    printf( "el Programa ha finalizado correctamente, se insertaron %d productos\n", posInsertOrDelete );

    
}

void *productor( void *param ) {
    srand( time( NULL ) );
    while( 1 ) {
        pthread_mutex_lock( &mutex );
            if( posInsertOrDelete <= 19 ) {
                printf( "Insertando producto en la posición: %d\n", posInsertOrDelete );
                productos[ posInsertOrDelete ] = 1;
                posInsertOrDelete++;
                pthread_cond_broadcast( &cond );
            }else{
                printf( "Productor descansando\n" );
                pthread_cond_wait(&cond, &mutex);
            }
        pthread_mutex_unlock( &mutex );
        usleep( 500000 );
       
    }

    

}

void *consumidor( void *param ) {
    srand( time( NULL ) );

    while( 1 ) {
        pthread_mutex_lock( &mutex );
            if( posInsertOrDelete >= 1 ) {
                printf( "consumiendo producto en la posición: %d\n", posInsertOrDelete -1 );
                productos[ posInsertOrDelete -1 ] = -1;
                posInsertOrDelete--;
                pthread_cond_broadcast( &cond );
            } else{
                printf( "consumidor descansando\n");
                pthread_cond_wait(&cond, &mutex);
            }
        pthread_mutex_unlock( &mutex );
       
        usleep( 600000 );
       
    }


}

