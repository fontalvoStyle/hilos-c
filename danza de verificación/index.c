#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

int limitePista = 0;

int **pasosDeLosRobots; 
int cantidadDePasosPorRobot;
int *pista;

pthread_barrier_t mybarrier; 
pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *funcion_hilo( void *param );

int main( int cantidadArgumentos, char ** argumentos ) {

    FILE *file = fopen( argumentos[ 3 ], "r" );

    if( file != NULL ) {

        int cantidadDeRobots = atoi( argumentos[ 1 ] ); 

        pasosDeLosRobots = (int **) malloc( sizeof( int *) * cantidadDeRobots ); // cada fila será una secuencia de pasos de un robot

        pthread_barrier_init( &mybarrier, NULL, cantidadDeRobots ); //inicializando barrera

        
        //vamos a leer los pasos de los robots del archivo y guardarlos en "pasosDeLosRobots"
        
        char *posicion;
        char numero[2];
        printf( "Ingresa el número de pasos que realiza cada robot: " );
        scanf("%d", &cantidadDePasosPorRobot );

        pista = (int *) malloc( sizeof( int ) * cantidadDePasosPorRobot );
        
        for( int i = 0; i < cantidadDeRobots; i++) {

            fscanf( file ,"%s", numero ); //leyendo el primer caracter de la linea ejemplo 1:   2:  3: ....

            pasosDeLosRobots[ i ] = (int * ) malloc( sizeof( int ) * cantidadDePasosPorRobot );

            for( int j = 0; j < cantidadDePasosPorRobot; j++ ) { // el 4 es el número de pasos que realiza cada robot
                fscanf( file ,"%s", numero );
               pasosDeLosRobots[ i ][ j ] =  atoi( numero );
            }
            
            
        }

        fclose( file ); // cerramos el archivo

        //imprimiendo los pasos de los robots

        for( int i = 0; i < cantidadDeRobots; i++) {
            printf( "pasos del robot %d :", i +1 );
            for( int j = 0; j < cantidadDePasosPorRobot; j++ ) { 
               printf( "[ %d ]", pasosDeLosRobots[ i ][ j ] );
            }
            printf( "\n" );
               
        }


        limitePista = atoi( argumentos[ 2 ] );

        //creamos los robots

        pthread_t identificadoresRobots[ cantidadDeRobots ];

        for( int i = 0; i < cantidadDeRobots; i++ ) {

            int *copiaIndice = (int *) malloc( sizeof( int ) );
            *copiaIndice = i;
            pthread_create( &identificadoresRobots[ i], NULL, funcion_hilo, copiaIndice );
        }

        //esperamos a que los robots terminen de hacer su procesos de verificación

        for( int i = 0; i < cantidadDeRobots; i++ ) {
            pthread_join( identificadoresRobots[ i ], NULL );
        }

        printf( "Todos los robots han terminado de bailar y están comenzado a trabajar\n" );

        free( pasosDeLosRobots );

    }else {
        printf( "No hemos podido abrir el archivo\n" );
    }
}

void *funcion_hilo( void *param ) {
    int *indice = (int *)param;
    pthread_barrier_wait( &mybarrier ); // que todos los robots empiecen a bailar al tiempo
    int esperaPorPrimeraVez = 1;
    srand( time(NULL) );
    for( int i = 0; i < cantidadDePasosPorRobot; i++ ) {
        
        int pasoAEjecutar = pasosDeLosRobots[ *indice ][ i ];
        
        pthread_mutex_lock(&mutex);
        pista[ pasoAEjecutar ]++; 
        while( pista [ pasoAEjecutar ] > limitePista ){
           printf( "Robot %d está esperando para iniciar el paso %d\n", *indice + 1, pasoAEjecutar );
           pthread_cond_wait(&cond, &mutex);	
        }

        printf( "Robot %d ha inciado el paso %d\n", *indice + 1, pasosDeLosRobots[ *indice ][ i ] );
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
        sleep( rand() % 3 + 1 );
        printf( "Robot %d ha finalizado el paso %d\n", *indice + 1, pasosDeLosRobots[ *indice ][ i ] );
        pthread_mutex_lock(&mutex);
        pista[ pasoAEjecutar ]--; 
        pthread_mutex_unlock(&mutex);


    }

    printf( "Robot %d ha terminado el paso %d y está esperando a los demás\n", *indice + 1, cantidadDePasosPorRobot );
    pthread_barrier_wait( &mybarrier ); // que todos los robots empiecen a bailar al tiempo

}