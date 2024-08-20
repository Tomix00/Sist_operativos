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

    for ((i=5 ; i>0 ; --i))
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

if test -f nombres.csv; then rm nombres.csv; fi
if test -f atpplayers_ranked.out; then rm atpplayers_ranked.out; fi
if test -d serie; then rm -r serie; fi


titulo "1" "Muestra las lineas donde matchea lo indicado - 'model name'" "grep 'model name' /proc/cpuinfo"
grep 'model name' /proc/cpuinfo
sleep 7

titulo "2" "Cuenta cuantas lineas contiene el archivo" "grep 'model name' /proc/cpuinfo | wc -l"
echo -e "grep 'model name' /proc/cpuinfo - solo para visualizar que se esta contando:\n"
grep 'model name' /proc/cpuinfo
echo -e "\n"
grep 'model name' /proc/cpuinfo | wc -l
sleep 7

titulo "3" "Recorta una columna especifica, elimina los espacios en blanco y los guarda en un archivo nombes.csv" "cut -d';' -f2 heroes.csv | grep . >> nombres.csv "
cut -d';' -f2 heroes.csv | grep . >> nombres.csv
echo "Chequee el archivo nombres.csv en su directorio"
sleep 7

titulo "4" "Obtener fecha de min(minimas) y max(maximas) de temperaturas" "sort -k5 -n -t' ' weather_cordoba.in | ... | cut -d' ' -f1-3"
echo "fecha de temperatura maxima: "
echo "comando: sort -k5 -n -t' ' weather_cordoba.in | tail -n 1 | cut -d' ' -f1-3"
sort -k5 -n -t' ' weather_cordoba.in | tail -n 1 | cut -d' ' -f1-3
echo -e "\n"

echo "fecha de temperatura minima: "
echo "comando: sort -k5 -n -t' ' weather_cordoba.in | head -n 1 | cut -d' ' -f1-3"
sort -k5 -n -t' ' weather_cordoba.in | head -n 1 | cut -d' ' -f1-3
sleep 7

titulo "5" "Ordenar por ranking atpplayers.in" "sort -k3 -n -t' ' >> atpplayers_ranked.txt"
sort -k3 -n -t' ' atpplayers.in >> atpplayers_ranked.out
echo "Chequee el archivo atpplayers_ranked.txt en su directorio"
sleep 7

titulo "6" "Ordenar por puntos y desempatar por diferencia" "awk '{printf '%-15s|Ptos: %-4s|Dif: %s \n',1,2,7-8}' superliga.in | sort -t'|' -k2 -r -k3"
awk '{printf "%-15s|Ptos: %-4s|Dif: %s \n",$1,$2,$7-$8}' superliga.in | sort -t'|' -k2 -r -k3
sleep 7

titulo "7" "MAC address del equipo" "ip addr | grep 'link/ether'"
ip addr | grep "link/ether"
sleep 7

titulo "8" "Renombrar multiples archivos en una carpeta" "for f in serie/*srt; do mv -- '$f' '${f%_es.srt}.srt' ; done"
mkdir serie
touch serie/fma_S01E{1..10}_es.srt
ls serie
echo -e "\n"
for f in serie/*srt; do mv -- "$f" "${f%_es.srt}.srt"; done
ls serie
sleep 7



clear
echo "end of script"
