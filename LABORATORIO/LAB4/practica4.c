#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>
#include<string.h>

int main (int argc, char * argv[]){

    if ( argc != 2 ){
        printf("El nombre de paràmetres no és correcte.\n");
        return 1;
    }
    string modo;

    modo = atof(argv[1]);
    archivo = atof(argv[2]);

    if (modo == "malloc"){
        // Modo malloc


    } else if (modo == "mmap"){
        // Modo mmap
    } else {
        printf("Este modo no existe");
    }


    return 0;
}