#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>

#define NIVEL_ALTO 750
#define NIVEL_MEDIO 450
#define NIVEL_BAJO 0

#define NORMAL 0.0
#define SEC -0.15
#define PLUJOS 0.10

#define REC_ALTO 0.05
#define REC_MEDIO 0.20
#define REC_BAJO 0.05

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
void senyalparesud( int signo){
    printf("El pare ha rebut la senyal del fill sud");
}

// RECUPERACIÓN DEL AGUA
double base_recuperacion(double deposito){
    if (deposito > NIVEL_ALTO){ return REC_ALTO; }
    if (deposito < NIVEL_ALTO && deposito > NIVEL_MEDIO){ return REC_MEDIO; }
    return REC_BAJO;
}

double evento_meteorologico(void){
    int r = rand() % 100;
    if (r < 75){ return NORMAL;}
    else if (r < 90) { return SEC; }
    else { return PLUJOS; }
}

double retorna_limit(double deposit, double lim_a, double lim_b, double lim_m){
    if(deposit > NIVEL_ALTO){ return lim_a;}
    else if(deposit > NIVEL_MEDIO){ return lim_m;}
    else{ return lim_b;}
  
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

    double lim_b, lim_a, lim_m;
    int anys;
    lim_a = atof(argv[1]);
    lim_m = atof(argv[2]);
    lim_b = atof(argv[3]);
    anys = atoi(argv[4]);
    
    //Aquest printf és únicament per a comprovar que funciona correctament l'entrada de paràmetres.
    printf(" Limite Alto: %f\n Limite Medio: %f\n Limite Bajo: %f\n Años: %d\n", lim_a, lim_m, lim_b, anys);

    int parent_id, north_id, south_id;
    int p_south[2], p_north[2], p_southBack[2], p_northBack[2];
    double deposit = 1000.0;

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
            //
            close(p_south[0]); 
            close(p_south[1]);
            close(p_southBack[0]); 
            close(p_southBack[1]);
            close(p_north[1]);
            close(p_northBack[0]);

            signal(SIGUSR1, senyalnord);

            pause();

        } else if(south_id == 0){
                pause();;
                // Tanquem els pipes que no fem servir
                close(p_south[1]);
                close(p_north[0]); 
                close(p_north[1]);
                close(p_southBack[0]);
                close(p_northBack[0]); 
                close(p_northBack[1]);

                signal(SIGUSR1, senyalsud);
                
                pause();

        } else { // Pare

                // Tanquem els pipes que no fem servir
                close(p_north[0]);
                close(p_south[0]);
                close(p_northBack[1]);
                close(p_southBack[1]);
                signal(SIGUSR1, senyalparenord);
                signal(SIGUSR2, senyalparesud);
                if(i != 1){
                    deposit = deposit*(base_recuperacion(deposit) + evento_meteorologico());
                    if (deposit < 0) deposit = 0.0;                    
                }
                double limit = retorna_limit(deposit, lim_a, lim_b, lim_m);

                write(p_north[1], &limit, sizeof(double));

                kill(north_id,SIGUSR1);
                pause();

                waitpid(north_id, NULL, 0);
                waitpid(south_id, NULL, 0);
        }
    }

    return 0;
}
