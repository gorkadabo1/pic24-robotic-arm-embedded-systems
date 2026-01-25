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

//Declara e inicializa a 0s todos los flags
int flags[6] = {0, 0, 0, 0, 0, 0};

//Declaracion de variables que se utilizaran para gestionar los duties de los servos y calcular el valor medio del potenciometro
int sel_servo, mediaPot;

/**Declaracion e inicialización del vector en el que se almacenan todos los duties de los 5 servomotores. Para los ejes X, Y, Z, se inicializan 
 * a su posicion segura, y para el eje Y de la pinza y el servo encargado de abrir y cerra la pinza, se inicializan al valor medio del duty.
 */
int duties[5] = {seg_X, seg_Y, seg_Z, DutyMiddle, DutyMiddle}; //ciclos para 1.5ms con prescaler 8;

/**
 * Función principal del programa que inicializa los módulos y variables necesarias para el funcionamiento del sistema,
 * y luego entra en un bucle infinito donde se gestionan varias tareas en función del estado de diferentes flags.
 * @return 0
 */
int main() {
    
    //Llama a la funcion que se encarga de llamar al resto de funciones donde se incializan los modulos utilizados
    inicializarModulos();
    
    //Llama a la funcion que se encarga de inicializar las variables declaradas anteriormente (sel_servo y mediaPot)
    inicializarVariables();
    
    //Bucle infinto donde se gestionan las tareas
    while (1) {
        
        //Llamar a la funcion encargada de actualizar la pantalla LCD
        actualizarPantalla();
        
        /** 
         * Se llama a la funcion encargada de calcular la media si el flag 'flag_media' esta activado, para procesar y calcular las medias de 
         * las muestras analogicas recogidas.
         */ 
        if (flags[flag_media] == 1) {
            calcularMedia();
        }
        
        //Se llama a la funcion encargada de manejar el cronometro del sistema si el flag 'flag_crono' esta activado.
        if (flags[flag_crono] == 1) {
            cronometro();
        }
        
        //Se llama a la funcion encargada de actualizar los valores de los duties de los servos si el flag 'flag_val_servo' esta activado.
        if (flags[flag_val_servo] == 1) {
            actualizarValorServos();
        }

        //Se lama a la funcion encargada de cambiar el valor del duty correspondiente al servomotor del eje Y de la pinza si el flag 'flag_control' esta activado.
        if (flags[flag_control] == 0) {
            //Se manda como parametros el indice correspondiente del servomotor en cuestion, y el resultado del calculo que ajusta el valor leido
            //del potenciometro, al duty del servomotor.
            changeDuties(servoPinzaY, (mediaPot / 1023.f)*(DutyMax - DutyMin) + DutyMin);
        }
        
        //Llama a la funcion que mueve el brazo robotico a una posición segura si el flag 'flag_seg' está activado, y luego resetea el flag.
        if (flags[flag_seg] == 1) {
            posicionSegura();
            flags[flag_seg] = 0;
        }

        /**
         * Si el flag 'flag_control' esta a 0, que es indicador de que el brazo se tiene que mover respecto a las entradas analogicas, se llama
         * a la funcion correspondiente que abre o cierra la pinza, y actualiza el valor del duty en la matriz de Informacion
         */
        if (flags[flag_control] == 0) {
            moverPinza(flags[flag_pinza]);
            conversion(&Informacion[PWM5][pos_pwm], duties[servoPinzaX]);
        }
    }

    return (0);
}