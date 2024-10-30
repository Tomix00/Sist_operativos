<style>
    r {color: Red}
    o {color: Orange}
    g {color: Green}
    y {color: Yellow}
    to {color: Tomato}
</style>

<h1 style="text-align:center;">Sistemas Operativos - Lab 03 - Informe</h1>

<h2 style="color: Orange;text-align:center;">Estudio sobre xv6-riscv</h2>


### 1 - ¿Qué política de planificación utiliza <span style="color: Fuchsia">xv6-riscv</span> para elegir el próximo proceso a ejecutarse?

- xv6-riscv utiliza la política RR (Round Robin) asignando a cada proceso una porción de tiempo equitativa y ordenada.
- Se define un intervalo de tiempo denominado "quantum".
- La cola de procesos se estructura como una cola circular.
- El planiﬁcador la recorre asignando un "quantum" de tiempo a cada proceso.
- La organización de la cola es FIFO.
- Si el proceso agota su ráfaga de CPU antes de ﬁnalizar el Quantum, el planiﬁcador asigna la CPU inmediatamente a otro proceso.

---
### 2 - ¿Cúales son los estados en los que un proceso puede permanecer en <span style="color: Fuchsia">xv6-riscv</span> y qué los hace cambiar de estado?

En el archivo "<y>proc.h</y>" vemos que los estados en los que un proceso puede permanecer en xv6-rscv son:

- <to>UNUSED</to>: Representa un "espacio vacío" en la proc table. Por ejemplo, cuando un proceso termina, adquiere este estado.
- <to>USED</to>: Estado que toma un proceso al ser creado. Se le asigna una pagetable vacía, un PID, un contexto y una página de trapframe.
- <to>SLEEPING</to>: Representa un proceso que está “esperando” a ser despertado con la función wakeup, y que no está haciendo uso de la CPU.
- <to>RUNNABLE</to>: Representa un proceso en cola para ser ejecutado, que todavía no ha sido seleccionado por el scheduler.
- <to>RUNNING</to>: Representa un proceso que está actualmente corriendo en la CPU.
- <to>ZOMBIE</to>: Es el estado que adquiere un proceso hijo antes de que el padre llame a la función wait para enterarse de que el hijo finalizó, y luego del wait el estado del proceso hijo pasa a ser UNUSED.


El proceso cambia de estado cuando es llamado por distintas funciones. Analizando el archivo "<y>proc.c</y>" vemos que:


- La función procinit inicializa un proceso con el estado <to>UNUSED</to>.
- La función allocproc cambia el estado de <to>UNUSED</to> a <to>USED</to>.
- La función freeproc cambia el estado a <to>UNUSED</to>.
- La función userinit inicializa y luego cambia el estado a <to>RUNNABLE</to>.
- La función fork inicializa el proceso hijo y luego cambia su estado a <to>RUNNABLE</to>.
- La función exit cambia el estado a <to>ZOMBIE</to>.
- La función scheduler cambia el estado de <to>RUNNABLE</to> a <to>RUNNING</to>.
- La función yield cambia el estado a <to>RUNNABLE</to>.
- La función sleep cambia el estado a <to>SLEEPING</to>.
- La función wakeup cambia el estado de <to>SLEEPING</to> a <to>RUNNABLE</to>.
- La función kill cambia el estado de <to>SLEEPING</to> a <to>RUNNABLE</to>.

---
### 3 - ¿Qué es un "quantum"? ¿Dónde se define en el código? ¿Cuánto dura un "quantum" en <span style="color: Fuchsia">xv6-riscv</span>?

- Un quantum es un intervalo de tiempo que se le asigna a cada proceso para ejecutarse. Si el quantum de un proceso termina antes de que éste termine de ejecutarse, se realiza una interrupción del proceso.

- Está definido en el código en la función "timerinit" en el archivo "<y>start.c</y>"

```c
// ask the CLINT for a timer interrupt.
int interval = 1000000; // cycles; about 1/10th second in qemu.
```

- Un quantum dura 1.000.000 ciclos del clock, aproximadamente 100ms en qemu. 

---
### 4 - ¿En qué parte del código ocurre el cambio de contexto en <span style="color: Fuchsia">xv6-riscv</span>? ¿En qué funciones un proceso deja de ser ejecutado? ¿En qué funciones se elige el nuevo proceso a ejecutar?

- El cambio de contexto se ocurre en el archivo "<y>swtch.S</y>", en la función swtch. Se guardan los registros del proceso que deja la CPU, y se cargan los registros del proceso que entra en la CPU.

```S
# Context switch
#
#   void swtch(struct context *old, struct context *new);
# 
# Save current registers in old. Load from new.	
```

- Un proceso deja de ser ejecutado cuando se llama a las funciones *yield*, *sleep* o *exit*. Estas funciones internamente llaman a la función *sched*, que a su vez llama a la funcion *swtch*, volviendo al contexto del scheduler.

- En la funcion scheduler se elige el nuevo proceso a ejecutar, la funcion busca el próximo proceso <to>RUNNABLE</to>.

---
### 5 - ¿El cambio de contexto consume tiempo de un quantum?

- Si, se consume tiempo de “quantum” ya que antes de realizar el cambio de contexto se tiene que almacenar información en los registros del proceso en ejecución, para que cuando se realice nuevamente un cambio de contexto éste pueda reanudarse, está operación de almacenamiento es la que consume el tiempo del “quantum”.

---
<h2 style="color: Orange;text-align:center;">Experimento 1</h2>

### 1 - Describa los parámetros de los programas cpubench e iobench para este experimento (o sea, los define al principio y el valor de N. Tener en cuenta que podrían cambiar en experimentos futuros, pero que si lo hacen los resultados ya no serán comparables).

- Iobech es un código/experimento que nos permite ejecutar procesos del tipo I/O en nuestro contexto. Cada ejecución abre un archivo para escritura y luego escribe A veces( <to>IO_EXPERIMENT_LEN</to> ) el contenido de un buffer de tamaño B( <to>IO_OPSIZE</to> ), y repite las operaciones para lectura, es decir, operaciones de entrada/salida. A su vez tenemos la variable <to>N</to> que determina la cantidad de veces que se ejecuta el programa en su totalidad, esta será definida en la llamada a Iobench. En nuestro experimento definimos <to>IO_EXPERIMENT_LEN</to>=512, <to>IO_OPSIZE</to>=128 y <to>N</to>=40.

- Cpubench es un código/experimento que nos permite ejecutar procesos del tipo CPU en nuestro contexto. Cada ejecución de medición realiza A multiplicaciones( <to>CPU_EXPERIMENT_LEN</to> ) de matrices BxB( <to>CPU_MATRIX_SIZE</to> ), es decir, operaciones de cómputo intensivo. A su vez tenemos la variable <to>N</to> que determina la cantidad de veces que se ejecuta el programa en su totalidad, esta será definida en la llamada a Cpubench. En nuestro experimento definimos <to>CPU_EXPERIMENT_LEN</to>=256, <to>CPU_MATRIX_SIZE</to>=128 y <to>N</to>=40

### 2 - ¿Los procesos se ejecutan en paralelo? ¿En promedio, qué proceso o procesos se ejecutan primero? Hacer una observación cualitativa.

- Por un lado analizaremos el caso de procesos *CPUBound*. Viendo nuestra métrica elegida *KOpsPerTick*, vemos que en medida de que hay más ejecuciones de procesos *CpuBench*, *KOpsPerTick* al principio es baja y con el paso de las ejecuciones de los *N* experimentos va aumentando(dados *N* distintos para cada *CpuBench*). Esto puede ser porque al principio se ejecutan tantos procesos en paralelo como llamadas a *CpuBench* provocando que el *ElapsedTime* sea alto(y casi igual) para todos los procesos. Sabiendo que la cantidad de operaciones es constante para cual sea la ejecución del experimento concluimos que la causa del aumento de la métrica es el *ElapsedTime* que creemos proviene de la ejecución paralela de procesos. Nuestra hipótesis se confirma siguiendo la ejecución pues una vez terminadas las ejecuciones de *CpuBench* con *N* más chico, *KOpsPerTick* aumenta.

<center>
<h4><g> Experimento 1-C </g><h4>

| Pomedios            	| (CPU) Proceso 6 	|
|---------------------	|-----------------	|
| KOpsPerTick         	| 13067           	|
| Elapsed Time(Ticks) 	| 41              	|

</center>
<center>
<h4><g> Experimento 1-D </g><h4>

| Promedios            	| (CPU) Proceso 12 	| (CPU) Proceso 14 	| (CPU) Proceso 15 	|
|----------------------	|------------------	|------------------	|------------------	|
| KOpsPerTick          	| 4330             	| 4367             	| 4352             	|
| Elapsed Time (Ticks) 	| 124              	| 123              	| 123              	|

</center>

- Por otro lado, los procesos *IOBound* no presentan los mismos resultados. El rendimiento según la métrica *OpsPerTickX100* en el caso de una ejecución es muy similar a cuando se ejecutan varios experimentos(*IOBench*) a la vez. Probablemente las operaciones de cómputo sean paralelizadas pero estas son insignificantes en comparación con las operaciones IO en términos de tiempo. Podemos concluir entonces que las operaciones IO no son paralelizables como las de CPU.

<center>
<h4><g> Experimento 1-A </g><h4>

| Promedios            	| (OI) Proceso 4 	|
|----------------------	|----------------	|
| OpsPerTickX100       	| 782            	|
| Elapsed Time (Ticks) 	| 131            	|

</center>
<center>
<h4><g> Experimento 1-B </g><h4>

| Promedios            	| (IO) Proceso 11 	| (IO) Proceso 13 	| (IO) Proceso 14 	|
|----------------------	|-----------------	|-----------------	|-----------------	|
| OpsPerTickX100       	| 854             	| 834             	| 823             	|
| Elapsed Time (Ticks) 	| 124             	| 126             	| 127             	|

</center>

### 3 -¿Cambia el rendimiento de los procesos iobound con respecto a la cantidad y tipo de procesos que se estén ejecutando en paralelo? ¿Por qué?

- El rendimiento de procesos *IOBound* no se ve afectado significativamente cuando se ejecutan otros procesos del mismo tipo en paralelo. La cantidad de operaciones es constante y el tiempo tiene variaciones insignificantes. Nuestra métrica *OpsPerTickX100* no sufre cambios relevantes relacionados con la cantidad de procesos ejecutándose en el mismo momento.

<center>
<h4><g> Experimento 1-A </g><h4>

| Promedios            	| (OI) Proceso 4 	|
|----------------------	|----------------	|
| OpsPerTickX100       	| 782            	|
| Elapsed Time (Ticks) 	| 131            	|

</center>
<center>
<h4><g> Experimento 1-B </g><h4>

| Promedios            	| (IO) Proceso 11 	| (IO) Proceso 13 	| (IO) Proceso 14 	|
|----------------------	|-----------------	|-----------------	|-----------------	|
| OpsPerTickX100       	| 854             	| 834             	| 823             	|
| Elapsed Time (Ticks) 	| 124             	| 126             	| 127             	|

</center>

- En el caso de procesos *CPUBound* que se ejecutan en paralelo, el rendimiento del proceso *IOBound* se ve levemente modificado. La CPU es ocupada la mayor parte del tiempo por los procesos *CPUBound* lo que provoca que las pocas operaciones cpu que ejecuta el proceso *IOBound* tomen más tiempo en ejecutarse, aumentando así el *ElapsedTime* y disminuyendo levemente la métrica.

<center>
<h4><g> Experimento 1-A </g><h4>

| Promedios            	| (OI) Proceso 4 	|
|----------------------	|----------------	|
| OpsPerTickX100       	| 782            	|
| Elapsed Time (Ticks) 	| 131            	|

</center>
<center>
<h4><g> Experimento 1-E </g><h4>

| Promedios            	| (CPU) Proceso 23 	| (CPU) Proceso 25 	| (CPU) Proceso 26 	| (IO) Proceso 21 	|
|----------------------	|------------------	|------------------	|------------------	|-----------------	|
| KOpsPerTick          	| 4188             	| 4321             	| 4316             	| -               	|
| OpsPerTickX100       	| -                	| -                	| -                	| 767             	|
| Elapsed Time (Ticks) 	| 128              	| 124              	| 124              	| 133             	|

</center>

### 4 -¿Cambia el rendimiento de los procesos cpubound con respecto a la cantidad y tipo de procesos que se estén ejecutando en paralelo? ¿Por qué?

- El rendimiento de procesos *CPUBound* se ve afectado significativamente cuando se ejecutan otros procesos del mismo tipo en paralelo. La cantidad de operaciones es constante pero el tiempo que demora en terminar la ejecución del proceso es distinta. Observando nuestra métrica *KOpsPerTick* vemos que contra más procesos del tipo *CPUBound* se ejecutan en paralelo más chico es el valor pues *ElapsedTicks* es mayor, en cambio contra menos procesos se ejecutan vemos que el valor es más grande lo que significa que el rendimiento es mayor. A su vez es de notarse que la capacidad total de la Cpu es de aproximadamente 12.000 *KOpsPerTick* y esta se reparte entre las ejecuciones de *CpuBench* que se dan en paralelo y es por esto que rendimiento se ve reducido cuando hay más cantidad de ejecuciones de *CpuBench*.

<center>
<h4><g> Experimento 1-C </g><h4>

| Pomedios            	| (CPU) Proceso 6 	|
|---------------------	|-----------------	|
| KOpsPerTick         	| 13067           	|
| Elapsed Time(Ticks) 	| 41              	|

</center>
<center>
<h4><g> Experimento 1-D </g><h4>

| Promedios            	| (CPU) Proceso 12 	| (CPU) Proceso 14 	| (CPU) Proceso 15 	|
|----------------------	|------------------	|------------------	|------------------	|
| KOpsPerTick          	| 4330             	| 4367             	| 4352             	|
| Elapsed Time (Ticks) 	| 124              	| 123              	| 123              	|

</center>

- En el caso de procesos *IOBound* que se ejecutan en paralelo, el rendimiento del proceso *CPUBound* no se ve afectado significativamente. Esto se debe a que la cantidad de operaciones de cpu que ejecutan los procesos *IOBound* es mucho menor que la cantidad de los procesos *CPUBound*, permitiendo así que el proceso *CPUBound* ocupe la CPU la mayor parte del tiempo logrando que *ElapsedTime* sea mucho menor.

<center>
<h4><g> Experimento 1-C </g><h4>

| Pomedios            	| (CPU) Proceso 6 	|
|---------------------	|-----------------	|
| KOpsPerTick         	| 13067           	|
| Elapsed Time(Ticks) 	| 41              	|

</center>
<center>
<h4><g> Experimento 1-F </g><h4>

| Promedios            	| (IO) Proceso 7 	| (IO) Proceso 9 	| (IO) Proceso 10 	| (CPU) Proceso 5 	|
|----------------------	|----------------	|----------------	|-----------------	|-----------------	|
| OpsPerTickX100       	| 794            	| 794            	| 786             	| -               	|
| KOpsPerTick          	| -              	| -              	| -               	| 12.159          	|
| Elapsed Time (Ticks) 	| 170            	| 171            	| 173             	| 44              	|

</center>

### 5 -¿Es adecuado comparar la cantidad de operaciones de cpu con la cantidad de operaciones iobound?

- No son comparables pues varían mucho. Las operaciones de cpu son muy rápidas y es por eso que dado un mismo lapso de tiempo se pueden ejecutar muchas más operaciones de cpu que de io. Las operaciones de cpu solo dependen de la propia cpu en cambio las operaciones io requieren accesos a memoria los cuales son extremadamente lentos.
