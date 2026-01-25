/*
 * Fecha: 09/05/2024
 * Autores: Iñaki Azpiroz, Gorka Dabo, Unai Cartagena
 */

#include "p24HJ256GP610A.h"
#include "commons.h"
#include "utilidades.h"
#include "mensajes.h"
#include "LCD.h"

//Funcion que inicializa el timer 4
void inic_Timer4() {
    TMR4 = 0; // Inicializar el registro de cuenta
    PR4 = 12500; // Periodo del timer
    // Queremos que cuente 4 ms.
    // Fosc= 50 MHz (vease Inic_oscilator()) de modo que
    // Fcy = 25 MHz (cada instruccion dos ciclos de reloj)
    // Por tanto, Tcy= 40 ns para ejecutar una instruccion
    // Para contar 4 ms se necesitan 12500 ciclos.
    T4CONbits.TCKPS = 1; // escala del prescaler 1:8
    T4CONbits.TCS = 0; // reloj interno
    T4CONbits.TGATE = 0; // Deshabilitar el modo Gate

    T4CONbits.TON = 1; // puesta en marcha del timer
    IEC1bits.T4IE = 1; // Habilitar interrupcion 
}

//Funcion que apaga y deshabilita las interrupciones del timer 4.
void stop_Timer4() {
    T4CONbits.TON = 0;
    IEC1bits.T4IE = 0;

}

//Funcion que enciende y rehabilita las interrupciones del timer 4.
void resume_Timer4() {
    T4CONbits.TON = 1;
    IEC1bits.T4IE = 1;
}

// Variables en las que se almacena el numero de milisegundos, decisegundos, segunds y minutos transcurridos
unsigned int mili, deci, seg, min;

//Inicializacion de las variables del cronometro a 0
void inic_crono()
{
    mili = 0;
    deci = 0;
    seg = 0;
    min = 0;
}

/**
 * Rutina de atencion del timer 4, como interrumpe cada 4ms, se suma 4 unidades a la variable que almacena los milisegundos.
 * Se activa el flag crono para que el main (que estará comprobando su valor constantemente), llame a la funcion encargada de 
 * actualizar los valores correspondientes en la matriz de informacion. Para finalizar, se desactiva el flag IF para limpiar
 * el flag de interrupción del Timer 4, evitando interrupciones adicionales hasta que el timer vuelva a cumplir su ciclo.
 */
void _ISR_NO_PSV _T4Interrupt() {
    mili += 4;
    flags[flag_crono] = 1;
    IFS1bits.T4IF = 0;
}

/**
 * Esta función es la encargada de manejar el cronometro del sistema, actualizando el tiempo transcurrido en décimas de segundo, segundos 
 * y minutos. El cronómetro se activa mediante un flag que se establece en la interrupción del Timer 4. Cada vez que el cronómetro acumula 
 * 100 milisegundos, se incrementan las décimas de segundo. Al alcanzar 10 décimas, se incrementa un segundo, y de manera similar, al alcanzar 
 * 60 segundos se incrementa un minuto. Además, cada 100 milisegundos se conmuta el valor del led 7. Por otro lado, al final de la funcion,
 * se pone el flag del cronometro a 0 para indicar al main de que ya se han actualizado los valores correspondientes.
 */
void cronometro()
{
    if (mili >= 100) {
        mili = 0;
        LATAbits.LATA4 = !LATAbits.LATA4;
        deci++;
        conversion_tiempo(&Informacion[crono][posds], deci);
        if (deci >= 10) {
            seg++;
            deci = 0;
            conversion_tiempo(&Informacion[crono][posseg], seg);
            if (seg >= 60) {
                min++;
                seg = 0;
                conversion_tiempo(&Informacion[crono][posmin], min);
            }
        }
    }
    flags[flag_crono] = 0;
}


/**
 * La siguiente función es la encargada de reiniciar el cronómetro del sistema. Esto incluye restablecer el contador del Timer 4 y 
 * actualizar los valores de tiempo mostrados almacenados en la matriz de Informacion. 
 */
void reiniciar() {
    TMR4 = 0;
    inic_crono();
    conversion_tiempo(&Informacion[crono][posds], deci);
    conversion_tiempo(&Informacion[crono][posseg], seg);
    conversion_tiempo(&Informacion[crono][posmin], min);
}

/**
 * Esta función realiza un retardo en milisegundos usando el Timer 8. Para ello, primero se define la frecuencia del ciclo de instrucciones
 * que será de 25MHz en este caso. Luego, calcula el numero de ciclos necesarios para alcanzar el tiempo de retardo recibido como parametro,
 * enciende e inicializa el timer 8, y calcula a ver si es necesario el uso del prescaler. Por otro lado, se calcula el PR que será el numero
 * de ciclos necesarios para alcanzar el tiempo de retardo - 1, y se pone el flag IF a 0. Además, se enciende el temporizador y se espera hasta 
 * que se active el flag de interrupcion que indica que ya ha transcurrido el tiempo necesario. Tras esto, se apaga el timer.
 */
long Fcy = 25000000;
void Delay_ms(unsigned int ms) {
    unsigned long ticks = (unsigned long) ms * (Fcy / 1000); // Calcular ticks necesarios para el delay
    T8CON = 0;
    TMR8 = 0;

    if (ticks < 65536) {
        T8CONbits.TCKPS = 0;
    } else if (ticks < 65536 * 8) {
        T8CONbits.TCKPS = 1; // 1:8 Prescaler
        ticks /= 8;
    } else if (ticks < 65536 * 64) {
        T8CONbits.TCKPS = 2; // 1:64 Prescaler
        ticks /= 64;
    } else {
        T8CONbits.TCKPS = 3; // 1:256 Prescaler
        ticks /= 256;
    }

    PR8 = ticks - 1; // Configurar periodo para el delay
    IFS3bits.T8IF = 0;
    T8CONbits.TON = 1; // Encender el temporizador

    while (IFS3bits.T8IF == 0); // Esperar por el flag de interrupci?n
    T8CONbits.TON = 0;
}

/**
 * Esta función realiza un retardo en microsegundos usando el Timer 8. Su funcionamiento es exactamente el mismo que el del Delay_ms, pero a 
 * la hora de calcular el numero de ciclos necesarios para que se haga la espera, hay que dividir entre 1000000 en vez de entre 1000 ya que 
 * estamos utilizando microsegundos en vez de milisegundos.
 * @param us
 */
void Delay_us(unsigned int us) {
    unsigned long ticks = (unsigned long) us * (Fcy / 1000000);
    T8CON = 0;
    TMR8 = 0;

    // Seleccionar el prescaler
    if (ticks < 65536) {
        T8CONbits.TCKPS = 0;
    } else if (ticks < 65536 * 8) {
        T8CONbits.TCKPS = 1;
        ticks /= 8;
    } else if (ticks < 65536 * 64) {
        T8CONbits.TCKPS = 2;
        ticks /= 64;
    } else {
        T8CONbits.TCKPS = 3;
        ticks /= 256;
    }

    PR8 = ticks - 1;
    IFS3bits.T8IF = 0;
    T8CONbits.TON = 1;

    while (IFS3bits.T8IF == 0);

    T8CONbits.TON = 0;
}

/**
 * Funcion que inicializa el timer 2. Este timer es utilizado para gestionar los duties de los servomotores. Como el PR se va modificando en 
 * de los duties de los servomotores, el valor inicial del PR2 en esta funcion es irrelevante ya que se ira modificando en la rutina de atencion.
 */ 
void inic_Timer2() {
    TMR2 = 0; // Inicializar el registro de cuenta
    PR2 = 1; // Periodo del timer
    // Fosc= 50 MHz (vease Inic_oscilator()) de modo que
    // Fcy = 25 MHz (cada instruccion dos ciclos de reloj)
    // Por tanto, Tcy= 40 ns para ejecutar una instruccion

    T2CONbits.TCKPS = 1; // escala del prescaler 1:8
    T2CONbits.TCS = 0; // reloj interno
    T2CONbits.TGATE = 0; // Deshabilitar el modo Gate

    IEC0bits.T2IE = 1; // Habilitar interrupci?n 
    IFS0bits.T2IF = 0; // Desactivar flag de interrupcion
    T2CONbits.TON = 1; // puesta en marcha del timer
}

/**
 * Variable en el que almacenaremos el numero total de ciclos transcurridos, para que después de que se hagan todos los duties, 
 * se pueda calcular el numero de ciclos restantes para llegar a 20 ms que es el periodo establecido para los servomotores.
 */ 
int sumatorio;

/**
 * Rutina de atención del timer 2. Funciona mediante un switch-case en el que, dependiendo del servomotor seleccionado,
 * apaga una salida y enciende otra. La variable sel_servo (que es inicializada a 0 desde el main), determina cuál servomotor
 * está siendo controlado en ese momento. La función se encarga de gestionar los tiempos de activación de cada servomotor
 * para mantener un ciclo total de 20 ms, asignando a cada uno un período activo basado en sus respectivos valores de duty cycle.
 * 
 * Cada 'case' corresponde a un servomotor diferente:
 *   Primero apaga el pin de salida del servomotor anterior y enciende el pin del servomotor actual.
 *   Establece PR2 al valor de duty del servomotor actual.
 *   Acumula el valor del duty en sumatorio para calcular el tiempo total usado en el ciclo.
 *   Incrementa sel_servo para pasar al siguiente servomotor en el próximo ciclo de interrupción.
 *   Cuando se alcanza el último servomotor (case == 4), se desactiva su pin de salida y se ajusta PR2 al tiempo restante para completar 
 *   el ciclo de 20 ms.
 *   Finalmente, reinicia sel_servo a 0 para comenzar de nuevo el ciclo en la siguiente interrupción.
 *   Activa una bandera flag_val_servo para indicar que se ha actualizado la configuración de los servomotores, y que el main llame a la funcion
 *   correspondiente para actualizar la informacion de la matriz Informacion.
 *   Limpia el flag de interrupción de Timer 2 (T2IF) para permitir la siguiente interrupción.
 */
void _ISR_NO_PSV _T2Interrupt() {
    switch (sel_servo) {
        case 0:
            sumatorio = 0;
            LATDbits.LATD0 = 1;
            PR2 = duties[0];
            sumatorio += PR2;
            sel_servo++;
            break;

        case 1:
            LATDbits.LATD0 = 0;
            PR2 = duties[1];
            sumatorio += PR2;
            LATDbits.LATD1 = 1;
            sel_servo++;
            break;

        case 2:
            LATDbits.LATD1 = 0;
            PR2 = duties[2];
            sumatorio += PR2;
            LATDbits.LATD2 = 1;
            sel_servo++;
            break;

        case 3:
            LATDbits.LATD2 = 0;
            PR2 = duties[3];
            sumatorio += PR2;
            LATDbits.LATD3 = 1;
            sel_servo++;
            break;

        case 4:
            LATDbits.LATD3 = 0;
            PR2 = duties[4];
            sumatorio += PR2;
            LATDbits.LATD9 = 1;
            sel_servo++;
            break;

        default:
            LATDbits.LATD9 = 0;
            PR2 = PR20ms - sumatorio;
            sel_servo = 0;
            break;
    }
    flags[flag_val_servo] = 1;
    IFS0bits.T2IF = 0;
}

/**
 * Funcion encargada de cambiar el duty del servomotor indicado mediante el primer parametro. Comprueba que el valor recibido en el segundo
 * parametro, este dentro de los limites maximos y minimos de los duties, y si esto se cumple, se actualiza el valor del duty del servomotor
 * indicado.
 * @param index
 * @param value
 */
void changeDuties(int index, int value){
    if(value >= DutyMin && value <= DutyMax){
        duties[index] = value;
    }
}

/**
 * Funcion encargada de decrementar el valor del servomotor indicado mediante parametro. Esta funcion es llamda desde el UART, y reduce el valor
 * del duty del servomotor indicado en 50 unidades. Tambien comprueba si el valor resultante es menor que el duty minimo, para que en ese caso, 
 * se recalcule el valor del duty, al valor de duty minimo.
 * @param index
 */
void decrementar_servo(int index){
    if(duties[index] >= DutyMin){
        duties[index] -= DutyBorder;
    } else{
        duties[index] = DutyMin;
    }
}
    
/**
 * Funcion encargada de incrementar el valor del duty del servomotor indicado mediante parametro. Esta funcion es llamda desde el UART, y aumenta
 * el valor del duty del servomotor indicado en 50 unidades. Tambien comprueba si el valor resultante es mayor que el duty minimo, para que en 
 * ese caso, se recalcule el valor del duty, al valor de duty maximo.
 * @param index
 */
void incrementar_servo(int index){
    if(duties[index] <= DutyMax - DutyBorder){
        duties[index] += DutyBorder;
    } else{
        duties[index] = DutyMax;
    }
}