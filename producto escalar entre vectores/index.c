#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int *vector1; // vector 1
int *vector2; // vector 2
int taman; // tamaño de los vectores

int resultado = 0; // resultado de la multiplicación escalar de los dos vectores
int segmentos = 0; // cuantas posiciones debe calcular cada vector

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex para evitar la concurrencia al calcular "resultado"

void *funcion_hilo( void *param ); // función que utilizaran los hilos para sus calculos


int main() {

    FILE *file;
    file = fopen( "index.txt" , "r" );
    
    if( file != NULL ) {

        //leyendo tamaño de los vectores
        fscanf( file, "%d", &taman );

        //dando espacio en memoria a los vectores

        vector1 = ( int *) malloc( sizeof( int ) * taman ); 
        vector2 = ( int *) malloc( sizeof( int ) * taman ); 

        //llenando con datos el primer vector 

        for( int i = 0; i < taman; i++ ){

            fscanf( file , "%d" , &vector1[ i ] );
        }

        //llenando con datos el segundo vector

        for( int i = 0; i < taman; i++ ){

            fscanf( file , "%d" , &vector2[ i ] );
        }

        printf( "\nImprimiendo vector 1\n\n");

        for( int i = 0; i < taman; i++ ){

            printf( "[ %d ]", vector1[ i ]  );

        }

        printf( "\n\nImprimiendo vector 2\n\n");

        for( int i = 0; i < taman; i++ ){

            printf( "[ %d ]", vector2[ i ]  );
            
        }

        int cantidadDeHilos = 0; // Guarda la cantidad de hilos que ingreas el usaurio
    
        do{

            printf( "\n\nIngresa la cantidad de hilos ( menor que %d ): ", taman );
            scanf( "%d", &cantidadDeHilos );

        }while( cantidadDeHilos >= taman || taman % (cantidadDeHilos + 1) != 0 ); // condición para que a cada hilo le corresponda el mismo número de posiicones a calcular

        pthread_t hilos[ cantidadDeHilos ]; // array  que contiene los identificadores de hilos creados desde el main

        int indice; // termina siendo el indice que corresponde al hilo principal
        int totalHilos = cantidadDeHilos + 1; // la cantidad de hilos que ingresa el usuario más el hilo principal
        segmentos = taman / totalHilos;  //cantidad de posicones del vector resultante que debe calcular cada hilo
        
        //creando hilos

        for( indice = 0; indice < cantidadDeHilos; indice++ ) {

            int *copiaIndice = (int *) malloc( sizeof( int ) );
            *copiaIndice = indice;
            pthread_create( &hilos[ indice ], NULL , funcion_hilo, copiaIndice ); 
        }

        //esperando hilos

        for( int i = 0; i < cantidadDeHilos; i++ ){
            pthread_join( hilos[ i ], NULL ); 
        }

        // hilo principal haciendo la parte que le corresponde

        for( int i = indice; i < taman; i++ ) {
            resultado += vector1[ i ] * vector2[ i ];
        }

        //imprimiendo el resultado

        printf( "\n\n El resultado de la multiplicación escalar es : %d\n", resultado );
        
        //liberando memoria y cerrando archivo
        fclose( file );
        free( vector1 );
        free( vector2 );

        
    }else{
        printf( "No se pudo abrir el archivo\n" );
    }
    printf("\n");

}

void *funcion_hilo( void *param ){ 

    int *indice = (int *) param; //recibiendo la copia del indice

    int posInicio =  *indice * segmentos; // posición del vector en la que sebe empezar a recorrer
    int posFinal = posInicio + segmentos; // posición del vector en la que debe finalizar

    for( int i = posInicio; i < posFinal; i++ ) {

        /*
        como varios hilos estarán modificando la misma variable 
        debemos evitar que los hilos modifiquen el valor de "resultado al mismo tiempo"
        */
        pthread_mutex_lock( &mutex );

            resultado += vector1[ i ] * vector2[ i ];

        pthread_mutex_unlock( &mutex );
    }

    pthread_exit( 0 );

}