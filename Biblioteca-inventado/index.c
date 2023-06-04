#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>

typedef struct Libro{
    char *titulo;
    char *contenido;
    int editando;
    int leyendo;

}libro;

libro *libros;

int cantidadDeLibros;
int librosPorEscritor;
int cantidadDeEscritores;
int cantidadDeLectores;
int control = 1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCond = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *escritor( void *param );
void *lector( void *param );

void signalHandler( int sig ){
    control = -1;
}

int main() {

    //abrir el archivo
    FILE *file = fopen( "biblioteca.txt", "r" );

    if( file != NULL ) {

        signal( SIGINT, signalHandler );

        //leyendo la cantidad de libros
        fscanf( file, "%d", &cantidadDeLibros );

        //reservar memoria para guardar los libros
        libros = (libro *) malloc( sizeof( libro ) * cantidadDeLibros );

        //guardamos cada libro

        for( int i = 0; i < cantidadDeLibros; i++ ) {
            libros[i].titulo = (char *) malloc( sizeof( char ) * 20 );
            libros[i].contenido = (char *) malloc( sizeof( char ) * 20 );
            fscanf( file , "%s", libros[i].titulo );
            fscanf( file , "%s", libros[i].contenido );
            libros[ i ].editando = -1;
            libros[ i ].leyendo = -1;
        }

        //Imprimiendo libros

        for( int i = 0; i < cantidadDeLibros; i++ ) {
           printf("________________________\n");
           printf( "Titulo: %s\n", libros[ i ].titulo );
           printf( "Contenido: %s\n", libros[ i ].contenido );
        }

        printf( "\nIngresa la cantidad de escritores: " );
        scanf( "%d", &cantidadDeEscritores );
        printf( "\nIngresa la cantidad de lectores: " );
        scanf( "%d", &cantidadDeLectores );

        librosPorEscritor = cantidadDeLibros / cantidadDeEscritores;

        //creando escritores

        pthread_t idEscritores[ cantidadDeEscritores ];

        for( int i = 0; i < cantidadDeEscritores; i++ ) {
            int *copiaIndice = (int *)malloc( sizeof( int ) );
            *copiaIndice = i;
            pthread_create( &idEscritores[ i ], NULL, escritor, copiaIndice );
        }

        //creando lectores
        pthread_t idLector[ cantidadDeLectores];
        for( int i = 0; i < cantidadDeLectores; i++ ) {
            int *copiaIndice = (int *)malloc( sizeof( int ) );
            *copiaIndice = i;
            pthread_create( &idLector[ i ], NULL, lector, copiaIndice );
        }

        //esperando escritores

        for( int i = 0; i < cantidadDeEscritores; i++ ) {
            
            pthread_join( idEscritores[ i ], NULL );
        }

        //esperando lectores

        for( int i = 0; i < cantidadDeLectores; i++ ) {
            
            pthread_join( idLector[ i ], NULL );
        }

        fclose( file );
        free( libros );
        

    } else{
        printf( "No se ha abierto el archivo correctamente\n" );
        exit( EXIT_FAILURE ); 
    }

}

void *escritor( void *param ){

    int *indice = (int *)param;
   

    while (control == 1) {
       
       for( int i = 0; i < cantidadDeLibros; i++ ) {

            pthread_mutex_lock( &mutex );
            if( libros[ i ].editando == 1 || libros[ i ].leyendo == 1  ) { // lo están editando
                
                if( libros[ i ].editando == 1 ) {
                    printf( "El escritor %d ha intentado editar %s pero ya estaba siendo editado\n", *indice, libros[ i ].titulo );
                }else{
                    printf( "El escritor %d ha intentado editar %s pero estaba siendo leido\n", *indice, libros[ i ].titulo );
                }
                
                pthread_mutex_unlock( &mutex );
            }else{
                printf( "El escritor %d está editando %s\n", *indice, libros[ i ].titulo );
                libros[ i ].editando = 1;
                pthread_mutex_unlock( &mutex );
                usleep( 500000 );

                pthread_mutex_lock( &mutex );
                libros[ i ].editando = -1;   
                printf( "El escritor %d ha terminado de editar %s\n", *indice, libros[ i ].titulo );
                pthread_cond_broadcast( &cond );
                pthread_mutex_unlock( &mutex );  
            } 
       }
    }
   
    
}
void *lector( void *param ){
    int *indice = (int *)param;
    while( control == 1 ) {

        for( int i = 0; i < cantidadDeLibros; i++ ) {

            pthread_mutex_lock( &mutexCond );
            while( libros[ i ].editando == 1 ) {
                printf( "El lector %d está intentó leer %s pero estaba siendo editado, pasa a esperar\n", *indice, libros[ i ].titulo );
                pthread_cond_wait( &cond, &mutex );
            }

            printf( "El lector %d está leyendo %s\n", *indice, libros[ i ].titulo );
            libros[ i ].leyendo = 1;
            usleep( 200000 );
            libros[ i ].leyendo = -1;
            pthread_mutex_unlock( &mutexCond );
        
        }
    }
}