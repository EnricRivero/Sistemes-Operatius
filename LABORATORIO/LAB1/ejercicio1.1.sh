#!/bin/bash

primerCodi=-1;
segonCodi=-1;

if [ $# -ne 4 ];
then
    echo "Es requereix el següent format:
    ./exercici1.sh <csv> <CODE1> <CODE2> <YEAR>."
    exit 1;
fi

if [ ${#4} -ne 4 ];
then
    echo "El format de l'any és incorrecte, ha de ser un número de 4 xifres."
    exit 1;
fi

#Per tal de poder fer una funció iterativa es fa un vector amb cadascuna de les columnes del fitxer
tail -n +2 $1  > aux.csv
codis=($(awk -F',' '{print $2}' aux.csv))
anys=($(awk -F',' '{print $3}' aux.csv))
shares=($(awk -F',' '{print $4}' aux.csv))

#Per tal d'evitar errors amb noms de paisos compostos es modifica el IFS
export IFS=$'
' ; paisos=($(awk -F',' '{print $1}' aux.csv))


#S'obtenen les files del primer i segon pais:
for (( i=0 ; i<=${#codis[@]} ; i++ ));
do
    if  [[ $2 == ${codis[$i]}  &&  $4 == ${anys[$i]} ]]  ;
    then
        primerCodi=$i;
    fi
    if [[ $3 == ${codis[$i]}  &&  $4 == ${anys[$i]} ]] ;
    then
        segonCodi=$i;
    fi
done

exitControl=0

if [ $primerCodi -lt 0 ] ;
then
    echo "No hi ha dades per CODE=$2 a YEAR=$4" ;
    exitControl=1 ;
fi

if [ $segonCodi -lt 0 ];
then
    echo "No hi ha dades per CODE=$3 a YEAR=$4" ;
    exitControl=1;
fi

if  [ $exitControl -eq 1 ] ;
then
    exit 1;
fi
primerPais=${paisos[$primerCodi]}
segonPais=${paisos[$segonCodi]}

echo "Any: $4"
echo "COD1: $2 ($primerPais) POBRESA: ${shares[$primerCodi]} %"
echo "COD2: $3 ($segonPais) POBRESA: ${shares[$segonCodi]} %"

primerPercentatge=(${shares[$primerCodi]})
segonPercentatge=(${shares[$segonCodi]})

#Es fa servir fitxer auxiliars per a poder recorre'ls amb awk i separar els decimals.
echo $primerPercentatge > aux1.csv
echo $segonPercentatge > aux2.csv
#En el cas de contindre decimals es procedeix amb la separació per mitja de awk
if [[ $primerPercentatge == *"."* ]] ;
then
    primeraAux=$( awk -F'.' '{print $1}' aux1.csv )
    segonaAux=$(  awk -F'.' '{print $2}' aux1.csv)

    primerPercentatgeCentenes=$(( ( $primeraAux * 10 ) + $segonaAux ))
else
    primerPercentatgeCentenes=$(( $primerPercentatge * 10 ))
fi

if [[ $segonPercentatge == *"."* ]] ;
then
    primeraAux=$( awk -F'.' '{print $1}' aux2.csv )
    segonaAux=$( awk -F'.' '{print $2}' aux2.csv )
    segonPercentatgeCentenes=$(( ( $primeraAux * 10 ) + $segonaAux ))
else
    segonPercentatgeCentenes=$(( $segonPercentatge * 10 ))
fi
#S'esborren els fitxer auxiliars
rm aux.csv
rm aux1.csv
rm aux2.csv

#Es fa el càlcul de la diferència i es torna a formatar.

if [ $primerPercentatgeCentenes -gt $segonPercentatgeCentenes ] ;
then
    diferencia=$(( $primerPercentatgeCentenes - $segonPercentatgeCentenes )) ;
    auxDif1=$(( $diferencia / 10 ))
    auxDif2=$(( ( $diferencia - ( $auxDif1 * 10 ) ) ))
    diferencia=$( echo "$auxDif1.$auxDif2" )
    major=$2

elif [ $primerPercentatgeCentenes -eq $segonPercentatgeCentenes  ] ;
then
    diferencia=0.0 ;
    major=EMPAT

 
else
    diferencia=$(( $segonPercentatgeCentenes - $primerPercentatgeCentenes )) 
    auxDif1=$(( $diferencia / 10 ))
    auxDif2=$(( ( $diferencia - ( $auxDif1 * 10 ) ) ))
    diferencia=$( echo "$auxDif1.$auxDif2" )
    major=$3
fi
echo "Diferència ( $2 - $3 ): $diferencia punts percentuals."
echo "País amb major índex de pobresa: $major ."
exit 0
