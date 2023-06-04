#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

const int true = 1;
const int false = -1;

int cantidadDeClientes;
int sillasDisponibles;
int sillaDeServicioOcupada = false;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCond = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_barrier_t 

int  sentarseEnSillaDeServicio( ) {

    int pudoSentarse = false;

    pthread_mutex_lock( &mutex );
        if( sillaDeServicioOcupada == false ){
            sillaDeServicioOcupada == true;
            pudoSentarse = true;
        }
    pthread_mutex_unlock( &mutex );

    return pudoSentarse;

}

void bajarseDeLaSillaDeServicio(){

    pthread_mutex_lock( &mutex );
        if( sillaDeServicioOcupada == true ){
            sillaDeServicioOcupada == false;
            pthread_cond_broadcast( &cond );
        }
    pthread_mutex_unlock( &mutex );

}

int sentarseEnSillaDeEspera() {

    int pudoSentarse = false;

    pthread_mutex_lock( &mutex );
        if( sillasDisponibles > 0 ){
           sillasDisponibles--;
           pudoSentarse = true;
        }
    pthread_mutex_unlock( &mutex );

    return pudoSentarse;

}

void cortarCabelloObarba( ) {
    sleep( 3 );
}

int  irse( int indice ){
    printf( "El cliente %d se ha ido\n", indice );
}

void esperar() {

    pthread_mutex_lock( &mutexCond );
        pthread_cond_wait( &cond, &mutexCond );
    pthread_mutex_unlock( &mutexCond );

}



void *cliente( void *param );

int main() {

    FILE *file = fopen( "barberia.txt", "r" );

    if( file != NULL ) {

        fscanf( file, "%d", &cantidadDeClientes );
        fscanf( file , "%d", &sillasDisponibles );

        pthread_t idClientes[ cantidadDeClientes ];

        for( int i = 0; i < cantidadDeClientes; i++ ) {
            int *copiaIndice = ( int *) malloc( sizeof( int ) );
            *copiaIndice = i; 
            pthread_create( &idClientes[ i ], NULL, cliente, copiaIndice );
        }

        for( int i = 0; i < cantidadDeClientes; i++ ) {
            pthread_join( idClientes[ i] , NULL );
        }

        printf( "Programa finalizado correctamente\n" );

    }else{
        printf( "NO hemos podido abrir la barberia\n" );
    }


}

void *cliente( void *param )  {

    int *idCliente = (int * ) param;

    if( sentarseEnSillaDeServicio() == true ) {

        printf( "El cliente %d se ha sentado en la silla de servicio\n" , *idCliente );

        cortarCabelloObarba();
        bajarseDeLaSillaDeServicio();

        printf( "El cliente %d se ha cortado el cabello y/o rasurado, procede a seder la silla\n" , *idCliente );

        irse( *idCliente );

    }else{
        
        printf( "El cliente %d ha encontrado la silla de servicio ocupada y procede a buscar una silla de espera\n" , *idCliente );

        if( sentarseEnSillaDeEspera() == false ) {

            printf( "El cliente %d no ha encontrado sillas de espera disponibles\n" , *idCliente );

            irse( *idCliente );

        }else{

            printf( "El cliente %d procede a sentarse en una silla de espera\n" , *idCliente );

            do{

                esperar();

            }while( sentarseEnSillaDeServicio() == false );

            cortarCabelloObarba();
            bajarseDeLaSillaDeServicio();

            printf( "El cliente %d después de haber esperado, se ha cortado el cabello y/o rasurado\n" , *idCliente );

            irse( *idCliente );
        }

    }


}