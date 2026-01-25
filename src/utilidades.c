/*
 * Fecha: 09/05/2024
 * Autores: Iñaki Azpiroz, Gorka Dabo, Unai Cartagena
 */

#include "p24HJ256GP610A.h"
#include "timers.h"
#include "oscilator.h"
#include "LCD.h"
#include "CN.h"
#include "GPIO.h"
#include "UART2_RS232.h"
#include "mensajes.h"
#include "ADC.h"
#include "utilidades.h"
#include "commons.h"

// Funcion para obtener los caracteres de un valor de dos digitos (<100)
// Parametros:
// val, valor numerico del que se quieren obtener los caracteres ASCII
// dir, direccion en la que la funcion devuelve los dos codigos ASCII
// En dir estara el caracter de mas peso y en dir+1 el de menos peso

unsigned char tabla_carac[16]="0123456789"; //Vector con todos los digitos  
void conversion_tiempo (unsigned char * dir, unsigned int val)
{
    unsigned char dig;
    if (val>99) {
        while(1);    
    } else {
        dig=val/10;
        dig=tabla_carac[dig];
        *dir=dig;
        dir++;
        dig=val%10;
        dig=tabla_carac[dig];
        *dir=dig;
    }
}
	
/**
 * Este metodo pasa el valor de numerico recibido como parametro a una secuencia de caracteres y la almacena en la 
 * direccion recibida por parametro
 * @param dir Direccion donde se almacenara el resultado
 * @param val Valor numerico a convertir en caracteres
 */
void conversion (unsigned char * dir, unsigned int val)
{
    unsigned char dig1, dig2, dig3, dig4;
        dig1 = val / 1000; // Obtener el primer d?gito
        dig1 = tabla_carac[dig1]; // Convertir a caracter
        *dir = dig1; // Almacenar el primer d?gito
        dir++;

        dig2 = (val / 100) % 10; // Obtener el segundo d?gito
        dig2 = tabla_carac[dig2]; // Convertir a caracter
        *dir = dig2; // Almacenar el segundo d?gito
        dir++;

        dig3 = (val / 10) % 10; // Obtener el tercer d?gito
        dig3 = tabla_carac[dig3]; // Convertir a caracter
        *dir = dig3; // Almacenar el tercer d?gito
        dir++;

        dig4 = val % 10; // Obtener el cuarto d?gito
        dig4 = tabla_carac[dig4]; // Convertir a caracter
        *dir = dig4; // Almacenar el cuarto d?gito
    
}

/** Funcion para cambiar el modo en el que se controla el brazo. En caso de estar el control analogico activo, pasaria a desactivar
 *  este y activar el digital, y viceversa.
 * Cuando flag_control = 0 --> El control funciona por entradas analogicas 
 * Cuando flag_control = 1 --> El control funciona por entradas digitales
 */
void changeMode(){
    if(flags[flag_control] == 0){
        flags[flag_control] = 1;
    } else{
        flags[flag_control] = 0;
    }
}

//Variables locales utilizadas para el metodo de posicion segura
int despX, despY, despZ, gradualX, gradualY, gradualZ, dirX, dirY, dirZ;
/** Este metodo movera el brazo robotico a la posicion segura preestablecida.
 */
void posicionSegura() {
    /*Calcular el desplazamiento a hacer por el brazo. El brazo debera moverse en cada eje el resultado de la resta de
     * la posicion destino menos la posicion en la que se encuentra. Asi, el valor resultante sera "lo que le queda" para llegar
     hasta la posicion segura*/
    despX = seg_X - duties[EJE_X];
    despY = seg_Y - duties[EJE_Y];
    despZ = seg_Z - duties[EJE_Z];
    /*Se controla el signo del desplazamiento, para saber si el desplazamiento debe ser hacia uno u otro lado. En caso de ser 
    * positivo, se guardara el valor 1. De lo contrario, el valor -1. Esto nos permite tener un control de hacia donde tendra
     * que moverse el brazo.
    */
    dirX = (despX > 0) ? 1 : -1;
    dirY = (despY > 0) ? 1 : -1;
    dirZ = (despZ > 0) ? 1 : -1;
    
    //En este while comprobamos si se ha alcanzado la posicion segura. De no ser asi, iterara sobre el bucle
    while (despX != 0 || despY != 0 || despZ != 0) {
        /**Comprueba si el desplazamiento en cada eje correspondiente ya se ha realizado, con las clausulas if.
        * En caso de que el desplazamiento no haya terminado, se cambia el valor del duties del servomotor correspondiente. 
        *Para que el movimiento sea gradual, se va sumando '1' o '-1', dependiendo del signo del desplazamiento
        */
        if (despX != 0) {
             
            changeDuties(EJE_X, duties[EJE_X]+dirX);
            despX-=dirX;
        }
        
       if (despY != 0) {
            changeDuties(EJE_Y, duties[EJE_Y]+dirY);   
            despY-=dirY;
        }
       
       if (despZ != 0) {
            changeDuties(EJE_Z, duties[EJE_Z]+dirZ);
            despZ-=dirZ;
        }
        //Delay de medio microsegundo para que el movimiento del brazo no sea tan rapido
        Delay_us(500);
    }

}

//Aqu? inicializamos los m?dulos utilizados para el proyecto
void inicializarModulos(){
    inic_oscilator();
    inic_joystick();
    inic_brazo();
    inic_Timer4();
    Init_LCD();
    inic_leds();
    inic_ADC1();
    inic_pulsadores();
    inic_crono();
    inic_UART2();
    inic_CN();
    inic_pwm();
    inic_Timer2();
}
    
//Aqu? inicializamos las variables utilizadas en el main y otro source file
void inicializarVariables(){
    mediaPot =  0;
    sel_servo = 0;
}

/** Metodo utilizado para mover el servo que abre y cierra la pinza. En funcion del valor recibido, abrira o cerrara la pinza
 * @param flag: Valor que decidira la funcion a realizar.
 * flag = 1 --> Cerrar pinza
 * flag = 0 --> Abrir pinza
 */
void moverPinza(int flag) {
    if (flag == 1) {
        incrementar_servo(servoPinzaX);
    } else {
        decrementar_servo(servoPinzaX);
    }
}

