#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>

#define NIVEL_ALTO 750
#define NIVEL_MEDIO 450
#define NIVEL_BAJO 0

// SEÑALES
void senyalnord(int signo){
    printf("El fill nort ha rebut la senyal");
}
void senyalsud(int signo){
    printf("El fill sud ha rebut la senyal");
}
void senyalparenord ( int signo){
    printf("El pare ha rebut la senyal del fill nord");
}
void senyalpare ( int signo){
    printf("El pare ha rebut la senyal del fill sud");
}

// RECUPERACIÓN DEL AGUA
double base_recuperacion(double deposito){
    if (deposito > NIVEL_ALTO){ return 5.0; }
    if (deposito < NIVEL_ALTO && deposito > NIVEL_MEDIO){ return 20.0; }
    return 5.0;
}

double evento_metereologico(void){
    int r = rand() % 100;
    if (r < 75){ return 0.0;}
    else if (r < 90) { return -15.0; }
    else { return 10.0; }
}

// PROCESO DE LOS HIJOS
void proceso_hijo(){
    // Com els dos fills fan el mateix, executem aquesta funció per no repetir codi
}

int main (int argc, char * argv[]){

    if ( argc != 5){
        printf("El nombre de paràmetres no és correcte.\n");
        return 1;
    }

    int lim_b, lim_a, anys, lim_m;
    lim_a = atof(argv[1]);
    lim_m = atof(argv[2]);
    lim_b = atof(argv[3]);
    anys = atoi(argv[4]);

    //Aquest printf és únicament per a comprovar que funciona correctament l'entrada de paràmetres.
    printf(" Limite Alto: %d\n Limite Medio: %d\n Limite Bajo: %d\n Años: %d\n", lim_a, lim_m, lim_b, anys);

    int parent_id, north_id, south_id;
    int p_south[2], p_north[2], p_southBack[2], p_northBack[2];

    pipe(p_south);
    pipe(p_north);

    pipe(p_southBack);
    pipe(p_northBack);

    parent_id = getpid();
    north_id = fork();
    south_id = fork();
    for (int i = 1; i <= anys ; i++){

        if (north_id == 0) { // Fill Nord

            // Tanquem els pipes que no fem servir
            close(p_north[1]);
            close(p_south[0]); close(p_south[1]);
            close(p_northBack[0]);
            close(p_southBack[0]); close(p_southBack[1]);

            signal(SIGUSR1, senyalnord);

            // TO DO

        } else if(south_id == 0){

                // Tanquem els pipes que no fem servir
                close(p_south[1]);
                close(p_north[0]); close(p_north[1]);
                close(p_southBack[0]);
                close(p_northBack[0]); close(p_northBack[1]);

                signal(SIGUSR1, senyalsud);

        } else { // Pare

                // Tanquem els pipes que no fem servir
                close(p_north[0]);
                close(p_south[0]);
                close(p_northBack[1]);
                close(p_southBack[1]);

                // TO DO

                waitpid(north_id, NULL, 0);
                waitpid(south_id, NULL, 0);
        }
    }

    return 0;
}
