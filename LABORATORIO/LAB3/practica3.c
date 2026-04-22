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


//Variables Globals de control:
volatile sig_atomic_t rebut_pare = 0;
volatile sig_atomic_t rebut_nord = 0;
volatile sig_atomic_t rebut_sud = 0;

// Handlers de Senyals:
void senyalnord(int signo){
    rebut_nord = 1;
}
void senyalsud(int signo){
    rebut_sud = 1;
}
void senyalparenord ( int signo){
    rebut_pare = 1;
}
void senyalparesud( int signo){
    rebut_pare = 1;
}

// RECUPERACIÓN DEL AGUA
double base_recuperacion(double deposito){
    if (deposito > NIVEL_ALTO){
        printf("[Event] Índex de recuperació aplicat: 5%%.\n");
        return REC_ALTO;
    }
    if (deposito < NIVEL_ALTO && deposito > NIVEL_MEDIO){
        printf("[Event] Índex de recuperació aplicat: 20%%. \n");
        return REC_MEDIO; 
    }
    printf("[Event] Índex de recuperació aplicat: 5%%. \n");
    return REC_BAJO;
}

//Meteorologia:
double evento_meteorologico(void){
    int r = rand() % 100;
    if (r < 75){
        printf("[Event] Any amb plujes normals.\n");
        return NORMAL;
    }
    else if (r < 90) {
        printf("[Event] Any sec.\n");
        return SEC;
    }
    else {
        printf("[Event] Any plujós.\n");
        return PLUJOS;
    }
}

double retorna_limit(double deposit, double lim_a, double lim_b, double lim_m){
    if(deposit > NIVEL_ALTO){ return lim_a;}
    else if(deposit > NIVEL_MEDIO){ return lim_m;}
    else{ return lim_b;}
  
}

// PROCESO DE LOS HIJOS
double proceso_hijo(double limit){
    // Falta implementar!
    return 0.0;
}

int main (int argc, char * argv[]){

    if ( argc != 5){
        printf("El nombre de paràmetres no és correcte.\n");
        return 1;
    }

    double lim_b, lim_a, lim_m,extraccio;
    int anys;
    lim_a = atof(argv[1]);
    lim_m = atof(argv[2]);
    lim_b = atof(argv[3]);
    anys = atoi(argv[4]);
    
    //Aquest printf és únicament per a comprovar que funciona correctament l'entrada de paràmetres.
    printf(" Limite Alto: %f\n Limite Medio: %f\n Limite Bajo: %f\n Años: %d\n", lim_a, lim_m, lim_b, anys);

    int parent_id, north_id, south_id;
    int p_south[2], p_north[2], p_southBack[2], p_northBack[2];
    double limit, diposit,recuperacio, total_nord,total_sud;

    //Configuració de Màscares:
    sigset_t mask, oldmask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    //Bloqueig de senyals:
    sigprocmask(SIG_BLOCK,&mask, &oldmask);

    diposit = 1000;
    total_nord = 0.0;
    total_sud = 0.0;

    pipe(p_south);
    pipe(p_north);

    pipe(p_southBack);
    pipe(p_northBack);

    parent_id = getpid();
    north_id = fork();

    // s'inicialitza south id per a evitar posteriors errors.
    south_id = -1;
    // s'evita que es creiin dos fills al procés nord.
    if (north_id != 0) south_id = fork();

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
                pause();
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

                printf("[Coordinador] Aigüa disponible a inici d'any:  %lf m3 \n",diposit);

                limit = retorna_limit(diposit, lim_a, lim_b, lim_m);
                printf("[Coordinador] Límit aplicat aquest any: %lf \n",diposit);
                write(p_north[1], &limit, sizeof(double));
                //S'envia senyal a fill
                kill(north_id,SIGUSR1);
                // Desbloquejem Senyals i esperem a rebre resposta del fill:
                while(!rebut_pare){
                    sigsuspend(&oldmask);
                }
                // Un cop reprès el control, recuperem el valor inicial:
                rebut_pare = 0;
                
                //Llegeix l'extracció del Nord
                read(p_northBack[0], &extraccio, sizeof(double));
                printf("[Coordinador] Regants nord sol·liciten %lf m3.\n",extraccio);

                if(extraccio >= diposit){
                    printf("[Coordinador] Sol·licitut aprovada per a Regants nord. \n");
                    diposit = diposit - extraccio;
                    total_nord += extraccio;
                    printf("[Coordinador] Aigüa concebuda a Regants nord: %lf m3. \n", extraccio);
                }else{
                    printf("[Coordinador] Sol·licitut denegada per a Regants nord.");
                }

                write(p_south[1], &limit, sizeof(double));

                kill(south_id,SIGUSR2);
                pause();
                
                //Llegeix l'extracció del sud
                read(p_southBack[0], &extraccio, sizeof(double));
                printf("[Coordinador] Regants sud sol·liciten %lf m3.\n",extraccio);

                if(extraccio >= diposit){
                    printf("[Coordinador] Sol·licitut aprovada per a Regants sud. \n");
                    diposit = diposit - extraccio;
                    total_sud += extraccio;
                    printf("[Coordinador] Aigüa concebuda a Regants sud: %lf m3. \n", extraccio);
                }else{
                    printf("[Coordinador] Sol·licitut denegada per a Regants sud.");
                }

                recuperacio =  evento_meteorologico() + base_recuperacion(diposit);
                printf("[Coordinador] Aigüa recuperada: %lf \n",recuperacio); 
                diposit = diposit + diposit*recuperacio;
                if (diposit < 0) diposit = 0.0; 
                printf("[Coordinador] Aigüa Disponible a final d'any: %lf \n", diposit);
                if( i == 5){
                    printf("La simulació ha finalitzat.\nAigüa otorgada a Regants nord: %lf m3\nAigüa otorgada a Regants sud: %lf m3\nTotal aigüa otorgada: %lf m3\nAigüa final al dipòsit: %lf m3\n ", total_nord, total_sud, total_nord + total_sud, diposit);
                   
                } 

                waitpid(north_id, NULL, 0);
                waitpid(south_id, NULL, 0);
        }
    }

    return 0;
}
