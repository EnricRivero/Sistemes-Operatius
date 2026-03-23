#!/bin/bash


if [ $# -ne 4 ];
then
    echo "Es requereix el format:
    ./exercici1_2.sh <csv> <CODE1> <CODE2> <YEAR>" ;
    exit 1 ;
fi

#S'inicialitzen variables amb els parametres d'entrada per comoditat
primerCodi=$2
segonCodi=$3
any=$4

#S'inicialitzen les variables dels paisos per saber si s'han trobat més endavant.
primerPais="-1"
segonPais="-1"

fitxer=$1

#Els awk recorren els fitxers a partir de la primera linea (NR>1) i si el codi i l'any coincideixen, es guarda a la variable.
pobresaPrimer=$(awk -F',' -v x="$primerCodi" -v y="$any" 'NR>1{
    if ( x == $2 && y == $3 ){
        print $4
    } 
}' $fitxer)

primerPais=$(awk -F',' -v x="$primerCodi" -v y="$any" 'NR>1{
    if ( x == $2 && y == $3 ){
        print $1
    } 
}' $fitxer)

pobresaSegon=$(awk -F',' -v x="$segonCodi" -v y="$any" 'NR>1{
    if ( x == $2 && y == $3 ){
        print $4
    } 
} ' $fitxer)

segonPais=$(awk -F',' -v x="$segonCodi" -v y="$any" 'NR>1{
    if ( x == $2 && y == $3 ){
        print $1
    } 
}' $fitxer)

#Les següents linies son per a comprovar si s'han trobat els paisos amb els codis i anys corresponents, en cas contrari finalitza el programa
controlSortida=0


if [ "$primerPais" == "-1" ];
then
    echo "El pais amb codi $primerCodi a l'any: $any no s'ha trobat" ;
    controlSortida=1
fi
if [ "$segonPais" == "-1" ] ;
then
    echo "El pais amb codi $segonCodi a l'any $any no s'ha trobat" ;
    controlSortida=1
fi

if [ $controlSortida -eq 1 ] ;
then
    exit 1
fi

#Fa el càlcul de la diferència a partir d'un codi en awk:
diferencia=$(awk -v x="$pobresaPrimer" -v y="$pobresaSegon" 'BEGIN{
    if ( x > y ){
        print x - y
    }else if (y > x) {
        print y - x
    } else {
        print 0.0 
    } 
}')

guanyador=$(awk -v x="$pobresaPrimer" -v y="$pobresaSegon" -v z="$primerCodi" -v t="$segonCodi" 'BEGIN{
    if ( x > y ){
        print z
    }else if (y > x) {
        print t
    } else {
        print "EMPAT" 
    } 
}')
echo "ANY: $any"
echo "COD1:$primerCodi ($primerPais) POBRESA: $pobresaPrimer %"
echo "COD2:$segonCodi ($segonPais) POBRESA: $pobresaSegon %"
echo "Diferència ($primerCodi - $segonCodi): $diferencia punts percentuals."
echo "País amb major pobresa: $guanyador"
exit 0
