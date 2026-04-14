#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#define MAX_INPUT 100
void sigusr1(int signo)
{
    printf("[signal] El fill ha rebut el SIGUSR1\n");
}

void sigusr2(int signo)
{
    printf("[signal] El pare ha rebut el SIGUSR2\n");
}

int main(void)
{
    int parent_pid, child_pid;
    int fd[2];

    char missatge[MAX_INPUT];
    char sense_vocals[MAX_INPUT];
    char resultat_final[MAX_INPUT];

    pipe(fd);
    child_pid = fork();
    if (child_pid == 0) { // fill

        /* PID del pare */
        parent_pid = getppid();

        /* Configurar senyals */
        signal(SIGUSR1, sigusr1);
        /* Esperar senyal */
        pause();
        /* Llegir dades */
        read(fd[0],missatge,sizeof(missatge));
        // Transformació 1
        int i = 0;
        while (missatge[i]) {
            char c = tolower(missatge[i]);
            if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
                sense_vocals[i] = '*';
            } else {
                sense_vocals[i] = missatge[i];
            }
            i++;
        }
        sense_vocals[i] = '\0';
        
        /* Escriure dades */
        write(fd[1],missatge, sizeof(missatge));
        /* Enviar senyal */
        kill(parent_pid,SIGUSR2);
        /* Esperar senyal */
        pause(); 
        /* Llegir dades */
        read(fd[0], missatge, sizeof(missatge)); 
        // Transformació 2
        int longitud = 0;
        for (int i = 0; missatge[i] != '\0'; i++) {
            if (missatge[i] != ' ' && missatge[i] != '\n') {
                longitud++;
            }
        }

        sprintf(resultat_final, "%s [longitud=%d]", missatge, longitud);

        /* Escriure dades */
        write( fd[1], resultat_final, sizeof(resultat_final));
        close(fd[1]);
        close(fd[0]);
        /* Enviar senyal */
        kill(parent_pid, SIGUSR2);
        exit(EXIT_SUCCESS);
    } else { // pare
        /* Configurar senyals */
        signal(SIGUSR2, sigusr2);

        /* Llegir entrada */

        printf("Introdueix una cadena: ");
        fgets(missatge, sizeof(missatge), stdin);

        /* Escriure dades */
        write(fd[1], missatge, sizeof(missatge));

        /* Enviar senyal */
        kill(child_pid, SIGUSR1);

        /* Esperar */
        pause();

        /* Llegir dades */
        read(fd[0],sense_vocals,sizeof(sense_vocals));
        printf("[pare] Cadena sense vocals rebuda: %s", missatge);

        /* Escriure dades */
        write(fd[1],sense_vocals,sizeof(sense_vocals));

        /* Enviar senyal */
        kill(child_pid, SIGUSR1);

        /* Esperar */
        pause();

        /* Llegir dades */
        read(fd[0],missatge, sizeof(missatge));
        close(fd[0]);
        close(fd[1]);
        printf("[pare] Cadena final rebuda: %s\n", missatge);
        
        wait(NULL);
        exit(EXIT_SUCCESS);
    }

    return 0;
}
