#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <signal.h>


const int true  = 1;
const int false = -1;
int pasosPorRobot = 4;
int cuposEnLaPista;

int cantidadDeRobots;

int **pasosDeLosRobots;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCond = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int  verificarPistaDeBaile() {
    int haycupos = false;
    pthread_mutex_lock( &mutex );
        if( cuposEnLaPista > 0 ){
            cuposEnLaPista--;
            haycupos = true;
        }
    pthread_mutex_unlock( &mutex );

    return haycupos;

}

int  bailar( int indiceRobot ){

    int pudoBailar = false;

    if( verificarPistaDeBaile( ) == true ){

        printf( "El robot %d ha comenzado a  bailar y ahora hay %d cupos\n", indiceRobot, cuposEnLaPista );
        
        for( int i = 0; i < pasosPorRobot; i++ ) {
            printf( "El robot %d está ejecutando el paso %d\n", indiceRobot, pasosDeLosRobots[ indiceRobot][ i ] );
            usleep( 500000 );
        }
    
        pthread_mutex_lock( &mutex );
            cuposEnLaPista++;
            printf( "El robot %d ha terminado de bailar y ahora hay %d cupos\n", indiceRobot, cuposEnLaPista );
            pthread_cond_broadcast( &cond );
        pthread_mutex_unlock( &mutex );
        
        pudoBailar = true;
    }

    return pudoBailar;
}

void esperar() {

    pthread_mutex_lock( &mutexCond);
    pthread_cond_wait( &cond, &mutexCond );
    pthread_mutex_unlock( &mutexCond );
}

void *robot( void *param );

int main( ) {

    FILE *file = fopen( "pasos.txt", "r" );

    if( file != NULL ){

        printf( "Por favor ingresa el número de robots que van a hacer la verificación: " );
        scanf( "%d", &cantidadDeRobots );

        printf( "Por favor ingresa el limite de la pista de baile: " );
        scanf( "%d", &cuposEnLaPista );

        pasosDeLosRobots = ( int ** ) malloc( sizeof( int * )  * cantidadDeRobots );

        //leer los pasos de los robots 
        char *paso = (char *) malloc( sizeof( char ) * 2 );

        for( int i = 0; i < cantidadDeRobots ; i++ ) {
            fscanf( file, "%s", paso); // simplemente rueda el putero
            pasosDeLosRobots[ i ] = (int *) malloc( sizeof( int ) * pasosPorRobot );
            for( int j = 0; j < pasosPorRobot; j++ ) {
                fscanf( file,  "%s", paso);
                pasosDeLosRobots[ i ][ j ] = atoi( paso );
            }
        }

        free( paso );
        fclose( file );
    

        for( int i = 0; i < cantidadDeRobots ; i++ ) {
            printf( "Pasos del robot %d: ", i );
            for( int j = 0; j < pasosPorRobot; j++ ) {
                printf("%d ", pasosDeLosRobots[i][j] );
            }
            printf("\n");
        }

        //creando robots

        pthread_t idRobots[ cantidadDeRobots ];

        for( int i = 0; i < cantidadDeRobots; i++ ) {
            int *copiaIndice = (int * ) malloc( sizeof( int ) );
            *copiaIndice  = i;
            pthread_create( &idRobots[ i ], NULL , robot, copiaIndice );
        }

        //esperamos hilos

        for( int i = 0; i < cantidadDeRobots; i++ ) {
           pthread_join( idRobots[ i ], NULL  );
        }

        printf( "El programa ha finalizado correctamente\n" );

        free( pasosDeLosRobots ); 
    
    }else{

        printf( "No se ha podido abrir el archivo correctamente\n" );

    }


}

void *robot( void *param ) {

    int *indice = (int *) param;
    int contador = 0;

    while ( contador < 2 ) {
        
        contador++;

        if( bailar( *indice ) == false ) {

            printf( "El rebot %d está esperando\n", *indice );

            do{
                esperar();

            }while ( bailar( *indice ) == false );

            sleep( 1 );
    
        }
 
      
    }
    pthread_exit( 0 );
}