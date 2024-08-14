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

# titulo "1-a" "Muestra informacion de la cpu" "cat /proc/cpuinfo"
# cat /proc/cpuinfo
# sleep 5

titulo "1" "Muestra las lineas donde matchea lo indicado - 'model name'" "grep 'model name' /proc/cpuinfo"
grep 'model name' /proc/cpuinfo
sleep 5

titulo "2" "Cuenta cuantas lineas contiene el archivo" "grep 'model name' /proc/cpuinfo | wc -l"
echo -e "grep 'model name' /proc/cpuinfo - solo para visualizar que se esta contando:\n"
grep 'model name' /proc/cpuinfo
echo -e "\n"
grep 'model name' /proc/cpuinfo | wc -l
sleep 5




clear
echo "end of script"
