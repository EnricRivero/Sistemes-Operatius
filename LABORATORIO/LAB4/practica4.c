#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>
#include<string.h>

typedef struct {
 char country[64];
 char code[8];
 int year;
 float life_expectancy;
} LifeRecord;

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

void modo_malloc(char *archivo,int sortTrue,int *total_records ,LifeRecord** records){
        
        //1. Reservar memoria inicial para 100 registros.
        LifeRecord *array = (LifeRecord *) malloc(100*sizeof(LifeRecord));

        //2. Leer cada línea mediante fgets.
        FILE *f = fopen(archivo,"r");
        int it, mida;
        char *st;
        it =  0;
        mida = 100;

        char * linia = (char*) malloc(100*sizeof(char));

        //6. Continuar hasta finalizar el fichero.
        while(fgets(linia,100,f)!= NULL){

            //4.Si la memoria se llena, duplicar la capacidad mediante realloc.
            if(it == mida){
                mida = mida*2;
                array = realloc(array,mida*sizeof(LifeRecord));
            }
            //3. Parsear datos usando strtok.
            for (int i = 0; i <4; i++){
                switch(i){

                    case 0:
                        st = strtok(linia,",");

                        //5. Guardar los registros en memoria.
                        for(int j = 0; st[j] != '\0'; j++){
                            array[it].country[j] = st[j];
                        }
                        break;

                    case 1:

                        st = strtok(NULL,",");
                        //5. Guardar los registros en memoria.
                         for(int j = 0; st[j] != '\0'; j++){
                            array[it].code[j] = st[j];
                        }
                        break;
                    case 2:

                        st = strtok(NULL,",");
                        //5. Guardar los registros en memoria.
                        array[it].year  = atoi(st);
                        break;

                    case 3:

                        st = strtok(NULL,",");
                        //5. Guardar los registros en memoria.
                        array[it].life_expectancy = atof(st);
                        break;
                }
            }
            it++;
        }
        LifeRecord max, min;
        float mean = 0.0;

        max = array[0];
        min = array[0];
        mean += array[0].life_expectancy;

        for (int i = 1; i < it+1 ; i++){
            if(array[i].life_expectancy > max.life_expectancy) max = array[i];
            if(array[i].life_expectancy < min.life_expectancy) min = array[i];
            mean += array[i].life_expectancy;
        }

        mean = mean/(it+1);

        printf(" Total registres: %d\n Mitja global: %f\n ",it+1,mean);
        printf("Major Valor: %s (%d) ->  %f\n",max.country,max.year,max.life_expectancy);
        printf("Menor Valor: %s (%d) ->  %f\n",min.country,min.year,min.life_expectancy);
        
        int anyIntro,index;
        inicialitzar_max_min(&max,&min);
        mean = 0.0;
        index = 0;

        printf("Introdueix un any: ");
        scanf("%d",&anyIntro);
        for(int i = 0; i < it+1 ; i++){
            if(array[i].year == anyIntro){
                if(array[i].life_expectancy > max.life_expectancy) max = array[i];
                if(array[i].life_expectancy < min.life_expectancy) min = array[i];
                mean += array[i].life_expectancy;
                index++;
            }
        }


        *total_records= mida;

        if(sortTrue){
            ordenar(array,mida);
        }
        *records = array;
}

void modo_mmap(char *archivo, int sortTrue, int *total_records, LifeRecord ** records){
        /*
        1. Abrir fichero con open().
        2. Obtener tamaño con fstat().
        3. Mapear fichero con mmap().
        4. Recorrer memoria detectando saltos de línea.
        5. Parsear cada línea.
        6. Liberar memoria con munmap(). 
        */
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


    return 0;
}


