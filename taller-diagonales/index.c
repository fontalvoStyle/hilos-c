#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

int filas = 0; // Número de filas de la matriz
int columnas = 0; // Número de columnas de la matriz
char **sopaDeLetras;
char **palabrasABuscar;
int cantidadDePalabrasABuscar = 0;

void *funcion_hilo( void *param );

typedef struct Mensaje{
    int fila;
    int columna;
    int indiceHilo;
} mensaje;

mensaje *mensajes;

int  buscarPalabraArribaAbajo( int fila , int columna, char *palabra ) {

    int limiteFila = filas -1;
    int limiteColumna = columnas - 1;

    if( fila == limiteFila  || columna == limiteColumna ){
        return 0;
    }

    int tamanPalabra = strlen( palabra );

    for( int p = 0; p < tamanPalabra; p++ ) {
        
       if(  columna > limiteColumna || fila > limiteFila || palabra[ p ] != sopaDeLetras[fila][columna] ){
            return 0;
        }
        fila++;
        columna++;

    }

    return 1;

}

int buscarPalabraAbajoArriba( int fila, int columna, char *palabra ) {
    
    if( fila == 0  || columna == 0 ){
        return 0;
    }

    int tamanPalabra = strlen( palabra );

    for( int p = 0; p < tamanPalabra; p++ ) {
        
       if(  columna < 0 || fila < 0 || palabra[ p ] != sopaDeLetras[fila][columna] ){
            return 0;
        }
        fila--;
        columna--;

    }

    return 1;

}


int main() {

    FILE *file = fopen( "index.txt", "r" ); // variable que apunta al archivo index.txt

    if( file != NULL ) { //si el archivo se abrió correctamente
    
        //leyendo filas y columnas de la matriz
        fscanf( file, "%d", &filas );
        fscanf( file, "%d", &columnas );

        //dando espacio en memoria a las filas de la sopa de letras
        sopaDeLetras = (char **) malloc( sizeof( char *) * filas );

        //asignando espacio en memora a las columnas de la sopa de letras y llenandola con los datos del archivo
        
        for( int i = 0; i < filas; i++ ) { // recorre las filas
            
            sopaDeLetras[ i ] = (char *) malloc( sizeof( char ) * columnas ); //asignando memoria a las columnas
            fscanf( file, "%s", sopaDeLetras[ i ] ); //leyendo toda una fila de letras
        
        }

        printf( "\nImprimeindo sopa de letras\n\n" );

        for( int i = 0; i < filas; i++ ) { // recorre las filas
            
            for( int j = 0; j < columnas; j++ ) { //recorre columnas
                printf( "[%c] ",sopaDeLetras[ i ][ j ] );
            }
            printf("\n");
        }

        fscanf( file, "%d", &cantidadDePalabrasABuscar); //leyendo la cantidad de palabras a buscar en el archivo

        palabrasABuscar = (char **) malloc( sizeof( char *) * cantidadDePalabrasABuscar ); // dando memoria para la cantidad de palabras

        //leyendo palabras a buscar

        char *palabra = (char *) malloc( sizeof( char ) * 20 );
        int tamanPalabra = 0;

        for( int i = 0; i  < cantidadDePalabrasABuscar; i++ ) {
            fscanf( file, "%s", palabra );
            tamanPalabra = strlen( palabra );
            palabrasABuscar[ i ] = ( char *) malloc( sizeof( char ) * tamanPalabra );
            strcpy( palabrasABuscar[ i ], palabra );
        }

        free( palabra ); //liberando memoria de palabra
        fclose( file ); //cerrando archivo

        printf( "\nLas palabras a buscar son: \n\n" );

        for( int i = 0; i < cantidadDePalabrasABuscar; i++ ) {
            printf( "%s\n", palabrasABuscar[ i ] );
        }

        //inicializando los mensajes
        mensajes = (mensaje * ) malloc( sizeof( mensaje ) * cantidadDePalabrasABuscar );

        for( int i = 0; i < cantidadDePalabrasABuscar; i++ ) {
            
            mensajes[ i ].fila = -1;
            mensajes[ i ].columna = -1;
            mensajes[ i ].indiceHilo = -1;
        }

        //creamos 2 hilos nuevos y pasandoles una copia del indice 

        pthread_t hilos[ 2 ];

        for( int i = 0; i  < 2; i++ ) {
            int *indice = (int *) malloc( sizeof(int ) );
            *indice = i;
            pthread_create( &hilos[ i ], NULL, funcion_hilo, indice );
        }

        //esperando los hilos creados

         for( int i = 0; i  < 2; i++ ) {
            pthread_join( hilos[ i ], NULL );
        }

        // una vez terminan los hilos , si estos encontraron algo debe estar en los mensajes

        printf( "_______________________________________________________________\n" );
        for( int i = 0; i < cantidadDePalabrasABuscar; i++ ){

            if( mensajes[ i ].fila != -1 ) {

                printf( "%s: en posición inicial %d-%d, hilo: %d ", palabrasABuscar[ i ], mensajes[ i ].fila, mensajes[ i ].columna, mensajes[ i ].indiceHilo );
                if(mensajes[ i ].indiceHilo == 0 ){
                    printf( "(arriba - abajo)");
                }else{
                    printf( "(abajo - arriba)");
                }
                printf("\n");
            }

        }

        free( sopaDeLetras ); //liberando memoria de la sopa de letras
        free( palabrasABuscar ); //liberando memoria de las palabras a buscar
        free( mensajes ); //liberando memoria de los mensajes
        

    }else{
        printf( "No se ha podido abrir el archvio\n" );
    }

}

void *funcion_hilo( void *param ) {

    int *indice = (int*) param;

    for( int i = 0; i < filas; i++ ){

        for( int j = 0; j < columnas; j++ ) {

            for( int k = 0; k < cantidadDePalabrasABuscar; k++ ){
                
                if( *indice == 0 ){

                    if( buscarPalabraArribaAbajo( i, j, palabrasABuscar[ k ] ) == 1 ){
                        mensajes[ k ].fila = i;
                        mensajes[ k ].columna = j;
                        mensajes[ k ].indiceHilo = *indice;
                        break;
                    }

                }else{

                    if( buscarPalabraAbajoArriba( i, j, palabrasABuscar[ k ] ) == 1 ){
                        mensajes[ k ].fila = i;
                        mensajes[ k ].columna = j;
                        mensajes[ k ].indiceHilo = *indice;
                        break;
                    }

                }

                        
            }
        }
    }

}