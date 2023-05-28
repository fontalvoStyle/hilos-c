#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

int *vector;
int taman; // tamaño del vector
int segmento = 0; // segmento del vector que le corresponde a cada hilo
int numeroAbuscar = 0;

//tipo de dato que nos va servir para enviar mensajes de los hilos que creamos al hilo principal
typedef struct Mensaje{
   int numeroDeVecesEncontrado;
   int *posicones;
} mensaje ;

// como son varios hilos que vana a enviar mensajes necesitamos un array de mensajes
mensaje *mensajes;


void *funcion_hilo( void *param ); //función que ejecutará cada hilo

int main( ) {

    FILE *file; // puntero que apunta al archivo que contiene los datos del vector

    file  = fopen( "index.txt", "r" ); // abrimos el archivo con permisos de lectura

    if( file != NULL ) { //verificamos que el archivo haya abierto correctamente

        fscanf( file, "%d", &taman ); //leemos el tamaño del vector desde un archivo

        vector = ( int *) malloc( sizeof( int ) * taman ); // damos espacio en memoria al vector

        //leemos los datos del archivo y vamos almacenandolos en el vector

        for( int i = 0; i < taman; i++ ) {
            fscanf( file, "%d", &vector[ i ] );
        }

        fclose( file ); //cerramos el archivo

        //imprimimos el vector
        printf( "\nVector\n\n" );
        for( int i = 0; i < taman; i++) {
            printf( "[ %d ]", vector[ i ] );
        }
        printf( "\n" );

        //pedimos al usuario que ingrese el núemer que desea buscar

        printf( "\nIngresa el número que deseas buscar: " );
        scanf( "%d", &numeroAbuscar );

        //pedimos al usuario que ingrese una cantidad de hilos

        int cantidadDeHilos = 0; //cantidad de hilos que ingresa el usuario

        do{
            printf( "Ingresa una cantidad de hilos: " );
            scanf( "%d", &cantidadDeHilos );

        }while( taman % cantidadDeHilos != 0);

        segmento = taman / cantidadDeHilos;

        mensajes = ( mensaje * ) malloc( sizeof( mensaje ) * cantidadDeHilos ); // damos memoria al array de mensajes

        //inicializamos cada mensaje porque también hay que darle memoria al array de posicones que tiene cada mensaje

        for( int i = 0; i < cantidadDeHilos; i++ ) {

            mensajes[ i ].numeroDeVecesEncontrado = 0;
            mensajes[ i ].posicones = (int *) malloc( sizeof( int ) * taman );
        }

        //creamos los hilos

        pthread_t hilos[ cantidadDeHilos ]; // guarda los identificadores de los hilos

        for( int i = 0; i < cantidadDeHilos; i++ ) {
            int *copiaIndice = ( int *) malloc( sizeof( int ) );
            *copiaIndice = i;
            pthread_create( &hilos[ i ], NULL, funcion_hilo, copiaIndice );
        }

        //esperamos los hilos

        for( int i = 0; i < cantidadDeHilos; i++ ) {
            pthread_join( hilos[ i ], NULL );
        }

        //imprimir mensajes

        for( int i = 0 ; i < cantidadDeHilos; i++ ) {

            if( mensajes[ i ].numeroDeVecesEncontrado != 0 ) {
                printf( "______________________________________________________\n");
                printf( "\nEl hilo %d encontró el número %d:  %d veces\n\n", i, numeroAbuscar, mensajes[ i ].numeroDeVecesEncontrado );
        
                printf( "Posciones: " );
                
                for( int j = 0; j < mensajes[ i ].numeroDeVecesEncontrado; j++ ) {
                    printf( " %d ", mensajes[ i ].posicones[ j ] );
                }
            }
        }
        printf( "\n" );

        free( vector ); //liberamos la memoria del vector
        free( mensajes ); //liberamos la memoria de los mensajes
    }

}

void *funcion_hilo( void *param ){

    int *indice = (int *)param; //obtenemos la copia del indice que se manda al hilo cuando se crea

    int posInicio = *indice * segmento; //posición del vector en la que va empezar a buscar 
    int posFinal = posInicio + segmento; //posición del vector en la que debe terminar de buscar
    int contador = 0; // sirve para ir guardando corectamente las posciones en donde se encuentra el dato

    for( int i = posInicio; i < posFinal; i++) {

        if( vector[ i ] == numeroAbuscar ){
            
            mensajes[ *indice ].numeroDeVecesEncontrado++;
            mensajes[ *indice ].posicones[ contador ] = i;
            contador++;
        }
    }

}