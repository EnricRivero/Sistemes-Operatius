#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<string.h>
#include <fcntl.h>


typedef struct {
 char country[64];
 char code[8];
 int year;
 float life_expectancy;
} LifeRecord;

size_t comptar_linies_mmap(char * line_start,char * file_end, char*line_end,struct stat st ){
        size_t linies;
        linies = 0;

        
        while(line_start < file_end && (line_end = memchr(line_start,'\n',file_end-line_start))!=NULL){

            /* Condicions del bucle:
             * line_start < file_end: l'inici de la linea a estudiar no és al final del mapejat.
             * memchr -> a partir d'un punt d'inici busca un determinat caràcter
             * a més a més memchr també comprova que no s'examinin bytes de més per mitjà d'un paràmetre limitant.
             * Paràmetres memchr: line_start -> Punter d'inici de la línea a estudiar ( inici: 0)
             * '\n' caràcter a buscar en la cadena.
             * file_end - line_start: bytes restants per acabar el fitxer, comprova que no hi hagi errors.
             */
          line_start  = line_end;
          line_start++; //El caràcter següent al del fi de línia és l'inici de la següent.
          linies++;
        }

        if(st.st_size > 0 && *(file_end -1) != '\n') linies ++;
        return linies;
}
void recorrer_mmap(LifeRecord** array,char * line_start, char* file_end, char* line_end, struct stat st){
    //Es fa servir de nou el bucle de la funció anterior.
    int it  = 0;
    size_t line_len;
    line_end = memchr(line_start,'\n',file_end - line_start);

    if(line_end == NULL){
        return;
    }
    //Això permet saltar-se la capçalera del fitxer csv.
    line_start = line_end;
    line_start ++;
    
    while(line_start < file_end ){

        line_end = memchr(line_start, '\n', file_end -line_start);

        if(line_end == NULL){
            line_end  = file_end;
        }
        line_len = line_end -line_start;
        if (line_len > 0) {
            // Al tractar-se de poca informació s'utilitza memòria estàtica, ja que és molt més ràpida en aquest sentit.
            char buffer[line_len + 1];

            memcpy(buffer,line_start,line_len); //Copia una cadena de caràcters a un buffer d'entrada.
            buffer[line_len] = '\0'; //Es col·loca el caràcter null per indicar el fi de la cadena.
            
            int scans =  sscanf(buffer, "%63[^,],%7[^,],%d,%f", (*array)[it].country, (*array)[it].code, &(*array)[it].year, &(*array)[it].life_expectancy);
            //La funció sscanf permet fer un scanf a partir d'un buffer de char d'entrada. 
            //Es fa servir (*array)[it] per tal d'indicar que entri primerament al punter del array i posteriorment vagi a la posició it.
            
            if(scans < 4){
                // Alguns països no tenen codi per algún motiu, provocant una pèrdua d'informació important.
                sscanf(buffer, "%63[^,],,%d,%f", (*array)[it].country, &(*array)[it].year, &(*array)[it].life_expectancy);
            }
            it++;

        }

        line_start = line_end;

        if(line_start< file_end) line_start ++;
    }
    
}


void inicialitzar_max_min(LifeRecord * max, LifeRecord * min){
        min->country[0] ='-';
        min->code[0] = '-';
        min->year = 0;
        min->life_expectancy = 10000000;
        max->country[0] = '-';
        max->code[0] = '-';
        max->year = 0;
        max->life_expectancy = 0;
}
int comparar_per_any(const void *a, const void *b) {
    const LifeRecord *ra = (const LifeRecord *)a;
    const LifeRecord *rb = (const LifeRecord *)b;
    return ra->year - rb->year;
}

void ordenar(LifeRecord *records, int total_records) {
    qsort(records, total_records, sizeof(LifeRecord), comparar_per_any);

    int limit = total_records < 10 ? total_records : 10;
    printf("%-40s %-8s %-6s %s\n", "País", "Código", "Año", "Esperanza de vida");
    printf("--------------------------------------------------------------------\n");
    for (int i = 0; i < limit; i++) {
        printf("%-40s %-8s %-6d %.2f\n", records[i].country, records[i].code, records[i].year,records[i].life_expectancy);
    }
}

void mostra_maxmin(LifeRecord *array,int it){
     LifeRecord max, min;
     float mean = 0.0;

     max = array[0];
     min = array[0];
     mean += array[0].life_expectancy;

    for (int i = 1; i < it ; i++){
        if(array[i].life_expectancy > max.life_expectancy) max = array[i];
        if(array[i].life_expectancy < min.life_expectancy) min = array[i];
        mean += array[i].life_expectancy;
    }

    mean = mean/(it);

    printf(" Total registres: %d\n Mitja global: %f\n ",it,mean);
    printf("Major Valor: %s (%d) ->  %f\n",max.country,max.year,max.life_expectancy);
    printf("Menor Valor: %s (%d) ->  %f\n",min.country,min.year,min.life_expectancy);
    
    int anyIntro,index;
    inicialitzar_max_min(&max,&min);
    mean = 0.0;
    index = 0;

    printf("Introdueix un any: ");
    scanf("%d",&anyIntro);
    for(int i = 0; i < it ; i++){
        if(array[i].year == anyIntro){
            if(array[i].life_expectancy > max.life_expectancy) max = array[i];
            if(array[i].life_expectancy < min.life_expectancy) min = array[i];
            mean += array[i].life_expectancy;
            index++;
        }
    }
    mean = mean/ index;
    printf(" Total registres: %d\n Mitjana de l'any:  %f\n ",index,mean);
    printf("Major Valor: %s (%d) ->  %f\n",max.country,max.year,max.life_expectancy);
    printf("Menor Valor: %s (%d) ->  %f\n",min.country,min.year,min.life_expectancy);
    


}

void modo_malloc(char *archivo,int sortTrue,int *total_records ,LifeRecord** records){
        
        //1. Reservar memoria inicial para 100 registros.
        LifeRecord *array = (LifeRecord *) malloc(100*sizeof(LifeRecord));

        //2. Leer cada línea mediante fgets.
        FILE *f = fopen(archivo,"r");
        int it, mida;
        char *st;
        it =  0;
        mida = 100;
        //Per a valors tant petits com 256 emprar memòria estàtica és més eficient que emprar memòria dinàmica.
        char linia[256];
        fgets(linia,100,f); //S'ignora la capçalera del fitxer csv

        //6. Continuar hasta finalizar el fichero.
        while(fgets(linia,100,f)!= NULL){

            //4.Si la memoria se llena, duplicar la capacidad mediante realloc.
            if(it == mida){
                mida = mida *2;
                array = realloc(array,mida*sizeof(LifeRecord));
                
            }
            char *st, *proxim;
            //3. Parsear datos usando strtok.
            for (int i = 0; i <4; i++){
                switch(i){

                    case 0:
                        st = strtok(linia,",");

                        //5. Guardar los registros en memoria.
                        strncpy(array[it].country,st,sizeof(array[it].country)-1); 
                        array[it].country[sizeof(array[it].country) - 1] = '\0'; // es col·loca l'últim element de country com a null per si de cas
                        break;

                    case 1:

                        st = strtok(NULL,",");
                        //5. Guardar los registros en memoria.
                        if(st[0] != '\0'){ 
                            strncpy(array[it].code,st,sizeof(array[it].code)-1);
                            array[it].code[sizeof(array[it].code) - 1] = '\0';
                        } else {
                            array[it].code[0] = '\0';
                        }
                        break;
                    case 2:

                        st = strtok(NULL,",");
                        //5. Guardar los registros en memoria.
                        array[it].year  = atoi(st);
                        break;

                    case 3:
                    
                        st = strtok(NULL,",\n"); // Es fa servir \n ja que no hi ha una coma al final d'aquesta columna si no un intro.
                        //5. Guardar los registros en memoria.
                        if(st){
                            array[it].life_expectancy = atof(st);
                        }
                        break;
                }
            }
            it++;
        }
        mostra_maxmin(array,it); 


        *total_records= it;

        if(sortTrue){
            ordenar(array,it);
        }
        *records = array;
        fclose(f);
}

void modo_mmap(char *archivo, int sortTrue, int *total_records, LifeRecord ** records){
        
        //1. Abrir fichero con open().
        int fd;
        struct stat st;
        fd = open(archivo,O_RDONLY);
        //2. Obtener tamaño con fstat().
        if(fstat(fd,&st) == -1){
            perror("fstat");
            return;
        }
        if(st.st_size == 0){
            printf("Fitxer buit");
            return;
        }

        //3. Mapear fichero con mmap().
        char *dades = mmap(NULL,st.st_size,PROT_READ,MAP_PRIVATE,fd,0);
        if(dades  == MAP_FAILED){
            printf("Error de mapeig");
            close(fd);
            return;
        }
        close(fd);
        
        char *line_start,*file_end,*line_end;
        line_start = dades;
        file_end = dades + st.st_size;
        size_t linies;

        //Càlcul del nombre de línies:
        linies = comptar_linies_mmap(line_start,file_end,line_end,st);
        linies -=1;
        //Comptar el nombre de línies permet fer un malloc fixe sense dependre de reallocs.
        LifeRecord * array = (LifeRecord*) malloc(linies*sizeof(LifeRecord));
        
        //4. Recorrer memoria detectando saltos de línea. 
        //5. Parsear cada línea.
        //
        recorrer_mmap(&array,line_start,file_end,line_end,st);
        *total_records = (int) linies;

        //6. Liberar memoria con munmap().
        if(munmap(dades,st.st_size) != 0){
            printf("Error fent munmap");
            return; 
        }
        
        mostra_maxmin(array,*total_records);

        if(sortTrue){
            ordenar(array,*total_records);
        }
        *records = array;


}

int main (int argc, char * argv[]){

    // 2 o 3 arguments
    if (argc < 3 || argc > 4) {
        printf("Uso: %s <malloc|mmap> <archivo.csv> [sort]\n", argv[0]);
        return 1;
    }

    char *modo = argv[1]; // Mode malloc o mmap
    char *archivo = argv[2]; // Archiu .csv amb les dades
    int sortTrue = 0;
    LifeRecord *records;
    int total_records;

    if(argc == 4) sortTrue = 1;

    if (strcmp(modo, "malloc") == 0) {
        modo_malloc(archivo, sortTrue, &total_records, &records);

    } else if (strcmp(modo, "mmap") == 0) {
        modo_mmap(archivo,sortTrue, &total_records , &records);
        
    } else {
        printf("Modo '%s' no existe. Usa 'malloc' o 'mmap'.\n", modo);
        return 1;
    }
    if(records != NULL){
        free(records);
    }


    return 0;
}


