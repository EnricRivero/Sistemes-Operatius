#!/bin/bash

if [ $# -ne 1 ] ;
then 
    echo "Arguments incorrectes, el format correcte és: ./exercici1.sh <fitxer>"
    exit 1
fi

fitxer=$1

if [ ! -f $fitxer ] ;
then
    echo "Error: El fitxer $fitxer no existeix o no es un fitxer regular."
    exit 1
fi


echo "Tipo: $(file $fitxer)"
echo "Nombre: $fitxer"
echo "Línies: $(wc -l $fitxer | awk '{print $1}')"
if [[ "$fitxer" == *.csv ]];
then
echo "Columnes: $(head -n 1 $fitxer | awk -F',' '{print NF}')"
fi
echo "Primeres 3 línies:"
head -n 3 $fitxer
echo "Últimes 3 línies:"
tail -n 3 $fitxer
echo " "

exit 0
