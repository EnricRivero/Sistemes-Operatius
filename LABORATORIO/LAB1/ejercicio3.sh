#!/bin/bash

# Comprovamos que se haya pasado el archivo de texto.
if [ $# -ne 1 ]; then 
	echo "El formato correcto es ./ejercicio3.sh <alerta_txt>"
	exit 1
fi
archivo=$1

if [ ! -f "$archivo"  ]; then 
	echo "No se ha encontrado el fichero"
	exit 1
fi

# Creamos el archivo con cabecera
echo "Fichero: $archivo" >> resumen_alerta.txt
echo "" >> resumen_alerta.txt
echo "Fichero: $archivo"

# Introducimos la lista de paises y sus códigos
tail -n +2 $archivo > aux.txt
paises=($(awk -F ';' '{print $1}' aux.txt))
rm aux.txt
echo "${paises[@]}" >> resumen_alerta.txt

# Imprimimos y añadimos al fichero el número de paises
echo "Paises en Alerta: ${#paises[@]}" >> resumen_alerta.txt
echo "Paises en Alerta: ${#paises[@]}"

# Si el fichero solo tiene una linea (la cabecera), devolvemos "N/A"
# En caso contrario, damos el código y el valor esperado (máxima pobreza y mínima electricidad)
max="N/A"
min="N/A"

if [ $(wc -l  < $archivo) -gt 1 ]; then
	max=$(awk -F';' 'NR>1 {if($3>max){max=$3; code=$1}} END{print code, max}' $archivo)
	min=$(awk -F';' 'NR>1 {if(min=="" || $4<min){min=$4; code=$1}} END{print code, min}' $archivo)
fi

# Imprimimos y añadimos al fichero los datos restantes (max pobreza, min electricidad, archivo generado)
echo "Mayor pobreza: $max" >> resumen_alerta.txt
echo "Mayor pobreza: $max"

echo "Menor acceso a electricidad: $min" >> resumen_alerta.txt
echo "Menor acceso a electricidad: $min"

echo "Generado: resumen_alerta.txt"
