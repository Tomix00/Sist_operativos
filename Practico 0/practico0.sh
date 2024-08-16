titulo(){

    EJ=$1
    INFO=$2
    COMAND=$3
    shift;shift;

    clear
    echo "ej:      $EJ"
    echo "info:    $INFO"
    echo "comando: $COMAND"
    sleep 3
    clear

    for ((i=3 ; i>0 ; --i))
    do
        echo "ej:      $EJ"
        echo "info:    $INFO"
        echo "comando: $COMAND"
        echo -e "\n"
        echo "$i"
        sleep 1
        clear
    done
}

titulo "1" "Muestra las lineas donde matchea lo indicado - 'model name'" "grep 'model name' /proc/cpuinfo"
grep 'model name' /proc/cpuinfo
sleep 5

titulo "2" "Cuenta cuantas lineas contiene el archivo" "grep 'model name' /proc/cpuinfo | wc -l"
echo -e "grep 'model name' /proc/cpuinfo - solo para visualizar que se esta contando:\n"
grep 'model name' /proc/cpuinfo
echo -e "\n"
grep 'model name' /proc/cpuinfo | wc -l
sleep 5

titulo "3" "Recorta una columna especifica, elimina los espacios en blanco y los guarda en un archivo nombes.csv" "cut -d';' -f2 heroes.csv | grep . >> nombres.csv "
cut -d';' -f2 heroes.csv | grep . >> nombres.csv
echo "Chequee el archivo nombres.csv en su directorio"
sleep 5

titulo "4" "Obtener fecha de min(minimas) y max(maximas) de temperaturas" "sort -k5 -n -t' ' weather_cordoba.in | ... | cut -d' ' -f1-3"
echo "fecha de temperatura maxima: "
echo "comando: sort -k5 -n -t' ' weather_cordoba.in | tail -n 1 | cut -d' ' -f1-3"
sort -k5 -n -t' ' weather_cordoba.in | tail -n 1 | cut -d' ' -f1-3
echo -e "\n"

echo "fecha de temperatura minima: "
echo "comando: sort -k5 -n -t' ' weather_cordoba.in | head -n 1 | cut -d' ' -f1-3"
sort -k5 -n -t' ' weather_cordoba.in | head -n 1 | cut -d' ' -f1-3
sleep 5


# fecha de maxima de maximas
# sort -k5 -n -t' ' weather_cordoba.in | tail -n 1 | cut -d' ' -f1-3
# fecha de minima de minimas
# sort -k5 -n -t' ' weather_cordoba.in | head -n 1 | cut -d' ' -f1-3


clear
echo "end of script"
