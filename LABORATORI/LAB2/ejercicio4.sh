#!/bin/bash

if [ $# -ne 2 ]
then
    echo "Format d'entrada incorrecte, el format correcte és ./exercici4.sh <fitxer entrada> <fitxer sortida>"
    exit 1
fi

fitxer=$1
sortida=$2
if [ ! -f $fitxer ]
then
    echo "Fitxer entrada KO"
    exit 1
else
    linies=$(wc -l $fitxer| awk '{print $1}')
    echo "Fitxer entrada OK. Línies d'entrada: $linies"
fi
#Primer S'eliminen els \r i els espais després de coma i finalment es passa el resultat a un grep per a que mostri només les línies amb contingut
sed -e 's/\r//g' -e 's/, */,/g' $fitxer | grep '.' > $sortida

if [ -f $sortida ]
then
    linies=$(wc -l $sortida| awk '{print $1}')
    echo "Fitxer sortida OK. Línies de sortida: $linies"
else
   echo "Fitxer sortida KO."
   exit 1
fi 
exit 0
