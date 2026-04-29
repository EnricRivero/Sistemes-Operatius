#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include<signal.h>
#include<time.h>

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
void senyalnord(int signo){ rebut_nord = 1; }
void senyalsud(int signo){ rebut_sud = 1; }
void senyalparenord ( int signo){ rebut_pare = 1; }
void senyalparesud( int signo){ rebut_pare = 1; }

// RECUPERACIÓN DEL AGUA
double base_recuperacion(double deposito){
    if (deposito > NIVEL_ALTO){
        printf("[Evento] Porcentaje de recuperacion base aplicado: 5%%\n");
        return REC_ALTO;
    }
    if (deposito <= NIVEL_ALTO && deposito > NIVEL_MEDIO){
        printf("[Evento] Porcentaje de recuperacion base aplicado: 20%%\n");
        return REC_MEDIO; 
    }
    printf("[Evento] Porcentaje de recuperacion base aplicado: 5%%\n");
    return REC_BAJO;
}

//Meteorologia:
double evento_meteorologico(void){
    int r = rand() % 100;
    if (r < 75){
        printf("[Evento] Año con lluvias normales\n");
        return NORMAL;
    }
    else if (r < 90) {
        printf("[Evento] Año seco\n");
        return SEC;
    }
    else {
        printf("[Evento] Año lluvioso\n");
        return PLUJOS;
    }
}

double retorna_limit(double deposit, double lim_a, double lim_m, double lim_b){
    if(deposit > NIVEL_ALTO){ return lim_a;}
    else if(deposit > NIVEL_MEDIO){ return lim_m;}
    else{ return lim_b;}
}

int main (int argc, char * argv[]){
    if ( argc != 5){
        printf("El nombre de paràmetres no és correcte.\n");
        return 1;
    }

    srand(time(NULL)); // Inicializar semilla aleatoria
    double lim_b, lim_a, lim_m, extraccio;
    int anys;
    lim_a = atof(argv[1]);
    lim_m = atof(argv[2]);
    lim_b = atof(argv[3]);
    anys = atoi(argv[4]);
    
    printf(" Limite Alto: %.2f\n Limite Medio: %.2f\n Limite Bajo: %.2f\n Años: %d\n", lim_a, lim_m, lim_b, anys);

    int parent_id, north_id, south_id;
    int p_south[2], p_north[2], p_southBack[2], p_northBack[2];
    double limit, diposit, recuperacio, total_nord, total_sud;

    //Configuració de Màscares:
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIG_USR1); // El sistema usa SIGUSR1 según enunciado [cite: 33]
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    diposit = 1000.0; [cite: 22]
    total_nord = 0.0;
    total_sud = 0.0;

    pipe(p_south); pipe(p_north);
    pipe(p_southBack); pipe(p_northBack);

    parent_id = getpid();
    north_id = fork(); [cite: 30]

    if (north_id != 0) {
        south_id = fork(); [cite: 30]
    }

    if (north_id == 0) { // --- UNIVERSO FILL NORD ---
        // Tanquem els pipes que no fem servir (Norte solo usa p_north y p_northBack)
        close(p_south[0]); close(p_south[1]);
        close(p_southBack[0]); close(p_southBack[1]);
        close(p_north[1]);     // Solo lee del padre
        close(p_northBack[0]); // Solo escribe al padre

        signal(SIGUSR1, senyalnord);

        for (int i = 1; i <= anys; i++) {
            while(!rebut_nord){
                sigsuspend(&oldmask); [cite: 35, 96]
            }
            rebut_nord = 0;

            read(p_north[0], &limit, sizeof(double));
            printf("[Regantes Norte PID %d] He recibido limite de: %.2f m3\n", getpid(), limit);
            extraccio = ((double)rand() / RAND_MAX) * limit; [cite: 81, 97]

            printf("[Regantes Norte PID %d] Solicito: %.2f m3\n", getpid(), extraccio);
            write(p_northBack[1], &extraccio, sizeof(double));
            kill(parent_id, SIGUSR1);
        }
        exit(0);

    } else if (south_id == 0) { // --- UNIVERSO FILL SUD ---
        // Tanquem els pipes que no fem servir
        close(p_north[0]); close(p_north[1]);
        close(p_northBack[0]); close(p_northBack[1]);
        close(p_south[1]);     // Solo lee del padre
        close(p_southBack[0]); // Solo escribe al padre

        signal(SIGUSR2, senyalsud); // El padre envía SIGUSR2 al sur en tu lógica

        for (int i = 1; i <= anys; i++) {
            while(!rebut_sud){
                sigsuspend(&oldmask);
            }
            rebut_sud = 0;

            read(p_south[0], &limit, sizeof(double));
            printf("[Regantes Sur PID %d] He recibido limite de: %.2f m3\n", getpid(), limit);
            extraccio = ((double)rand() / RAND_MAX) * limit;

            printf("[Regantes Sur PID %d] Solicito: %.2f m3\n", getpid(), extraccio);
            write(p_southBack[1], &extraccio, sizeof(double));
            kill(parent_id, SIGUSR2);
        }
        exit(0);

    } else { // --- UNIVERSO PARE (COORDINADOR) ---
        // Tanquem els pipes que no fem servir
        close(p_north[0]); close(p_south[0]);
        close(p_northBack[1]); close(p_southBack[1]);
        
        signal(SIGUSR1, senyalparenord);
        signal(SIGUSR2, senyalparesud);

        for (int i = 1; i <= anys; i++) {
            printf("\n* AÑO %d\n", i); [cite: 103]
            printf("[Coordinador] Agua disponible al inicio: %.2f m3\n", diposit); [cite: 104]

            limit = retorna_limit(diposit, lim_a, lim_m, lim_b);
            printf("[Coordinador] Limite aplicado este año: %.2f m3\n", limit); [cite: 105]

            // --- GESTIÓN NORTE ---
            write(p_north[1], &limit, sizeof(double));
            kill(north_id, SIGUSR1);
            while(!rebut_pare){ sigsuspend(&oldmask); }
            rebut_pare = 0;
            
            read(p_northBack[0], &extraccio, sizeof(double));
            printf("[Coordinador] Regantes Norte solicitan: %.2f m3\n", extraccio);
            if(extraccio <= diposit){ [cite: 84]
                printf("[Coordinador] Solicitud aprobada para Regantes Norte\n");
                diposit -= extraccio;
                total_nord += extraccio;
            } else { [cite: 85]
                printf("[Coordinador] Solicitud ajustada (deposito insuficiente) para Regantes Norte\n");
                total_nord += diposit;
                diposit = 0;
            }

            // --- GESTIÓN SUR ---
            write(p_south[1], &limit, sizeof(double));
            kill(south_id, SIGUSR2);
            while(!rebut_pare){ sigsuspend(&oldmask); }
            rebut_pare = 0;

            read(p_southBack[0], &extraccio, sizeof(double));
            printf("[Coordinador] Regantes Sur solicitan: %.2f m3\n", extraccio);
            if(extraccio <= diposit){
                printf("[Coordinador] Solicitud aprobada para Regantes Sur\n");
                diposit -= extraccio;
                total_sud += extraccio;
            } else {
                printf("[Coordinador] Solicitud ajustada (deposito insuficiente) para Regantes Sur\n");
                total_sud += diposit;
                diposit = 0;
            }

            // --- RECUPERACIÓN ---
            double pct_base = base_recuperacion(diposit); [cite: 57, 88]
            double pct_var = evento_meteorologico(); [cite: 63, 89]
            double pct_final = pct_base + pct_var;
            if (pct_final < 0) pct_final = 0; [cite: 69]

            double agua_recup = diposit * pct_final;
            diposit += agua_recup; [cite: 90]
            if (diposit > 1000) diposit = 1000.0; [cite: 22, 90]

            printf("[Coordinador] Agua recuperada: %.2f m3\n", agua_recup);
            printf("[Coordinador] Agua disponible al final del año: %.2f m3\n", diposit); [cite: 122]
        }

        // Finalización: el padre espera a los hijos fuera del bucle
        waitpid(north_id, NULL, 0); [cite: 99]
        waitpid(south_id, NULL, 0); [cite: 99]

        printf("\nLa simulacion ha finalizado.\n"); [cite: 206]
        printf("Agua concedida a Regantes Norte: %.2f m3\n", total_nord); [cite: 207]
        printf("Agua concedida a Regantes Sur: %.2f m3\n", total_sud); [cite: 208]
        printf("Total agua concedida: %.2f m3\n", total_nord + total_sud); [cite: 209]
        printf("Agua final en el deposito: %.2f m3\n", diposit); [cite: 210]
    }

    return 0;
}
