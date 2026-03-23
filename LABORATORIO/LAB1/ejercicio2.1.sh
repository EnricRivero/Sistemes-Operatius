#!/bin/bash


#COMPROVACIÓ: Paràmetres vàlids
if [ $# -ne 5 ]; then
    echo "Uso: ./ejercicio2.sh <poverty_csv> <services_csv> <YEAR> <POV_MIN> <ELEC_MAX>"
    exit 1
fi

#VARIABLES: Clean code
POV_CSV=$1
ELEC_CSV=$2
YEAR=$3
POV_MIN=$4
ELEC_MAX=$5

#COMPROVACIÓ: Existencia dels fitxers
if [ ! -f "$POV_CSV" ]; then
    echo "Error: El fichero de pobreza '$POV_CSV' no existe."
    exit 1
fi

if [ ! -f "$ELEC_CSV" ]; then
    echo "Error: El fichero de servicios '$ELEC_CSV' no existe."
    exit 1
fi

#Creació de l'informe (anomenat segons l'any)
OUTPUT_FILE="alerta_ods1_${YEAR}.txt"

#Override per posar la capçalera (>)
echo "Code; Entity; Poverty Rate; Electricity" > "$OUTPUT_FILE"

#Lectura dels fitxers amb awk (-F',' per separar per comes)(-v passem variables al awk)
awk -F',' -v yr="$YEAR" -v p_min="$POV_MIN" -v e_max="$ELEC_MAX" '
    #Primer fitcher
    #Només cert amb la lecutra del primer fitxer 
    #NR= Number of records:Contador global de lineas
    #FNR= File Number of Records: Contador especific de fitxer de lineas
    #Primer fitxer (electricitat)
    FNR == NR {
        if ($3 == yr && $4 != "" && $4 <= e_max) {
            #"Diccionari": Clave=Pais ($2), Valor=Dada ($4)
            elec[$2] = $4
        }
        next
    }
    #Segon fitxer(pobresa)
    {
        if ($3 == yr && $4 != "" && $4 >= p_min) {
            #Mirem si es en el dicc
            if ($2 in elec) {
                #Imprimimos les dades
                printf "%s; %s; %s; %s\n", $2, $1, $4, elec[$2]
            }
        }
    }
#Escribim en el fitxer final
' "$ELEC_CSV" "$POV_CSV" >> "$OUTPUT_FILE"

#Contem les lineas amb wc -l i li restem la linea de la capcelera
TOTAL_LINES=$(wc -l < "$OUTPUT_FILE")
COUNT=$((TOTAL_LINES - 1))

echo "Generado: $OUTPUT_FILE"
echo "Países detectados: $COUNT"

exit 0
