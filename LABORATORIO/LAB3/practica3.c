#include<stdio.h>
#include<stdlib.h>

int main (int argc, char * argv[]){

    if ( argc != 5){
        printf("El nombre de paràmetres no és correcte.\n");
        return 1;
    }
    int lim_b, lim_a, niv_b, anys;
    lim_b = atoi(argv[1]);
    lim_a = atoi(argv[2]);
    niv_b = atoi(argv[3]);
    anys = atoi(argv[4]);
    //Aquest printf és únicament per a comprovar que funciona correctament l'entrada de paràmetres.
    printf(" Límit Baix: %d\n Límit Alt: %d\n Nivell Baix: %d\n Anys: %d\n", lim_b, lim_a, niv_b, anys);
    return 0;

}
