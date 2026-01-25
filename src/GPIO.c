/*
 * Fecha: 09/05/2024
 * Autores: Iñaki Azpiroz, Gorka Dabo, Unai Cartagena
 */


#include "p24HJ256GP610A.h"
#include "commons.h"

//Metodo que inicializa los leds del microcontrolador.
void inic_leds() {
    TRISA = 0xff00; // Definir como salidas los 8 pines conectados a leds: RA7-RA0		  // El resto como entradas: 1 = In; 0 = Out
    LATA = LATA & 0xff00; // Apagar los leds
}

//Metodo que inicializa los pulsadores utilizados. 
void inic_pulsadores() {
    AD1PCFGH = 0xffff;     //Desactiva todos los pines, para despues activar los que utilizaremos
    TRISDbits.TRISD7 = 1;  //Establecer pin RD7 como entrada. Utilizado para establecer el modo de control del brazo
    TRISDbits.TRISD13 = 1; //Establecer pin RD13 como entrada. Utilizado para mover el brazo a la zona segura 
    TRISDbits.TRISD6 = 1;  //Establecer pin RD6 como entrada. Utilizado para scrollear en pantalla LCD
}

//Metodo que inicializa los pines utilizados para el joystick como entradas 
void inic_joystick() {
    TRISBbits.TRISB8 = 1; //Establece el pin RB8 como entrada. Utilizado para mover eje X del brazo con el joystick
    TRISBbits.TRISB9 = 1; //Establece el pin RB9 como entrada. Utilizado para mover eje Y del brazo con el joystick
    TRISBbits.TRISB10 = 1; //Establece el pin RB10 como entrada. Utilizado para mover eje Z del brazo con el joystick
    TRISBbits.TRISB2 = 1; 
}

//Metodo que inicializa los pines utilizados para los servomotores del brazo
void inic_brazo(){
    TRISDbits.TRISD0 = 0;   //Establecer el pin RD0 como salida. Utilizado para mover el servo 0
    TRISDbits.TRISD1 = 0;   //Establecer el pin RD1 como salida. Utilizado para mover el servo 1
    TRISDbits.TRISD2 = 0;   //Establecer el pin RD2 como salida. Utilizado para mover el servo 2
    TRISDbits.TRISD3 = 0;   //Establecer el pin RD3 como salida. Utilizado para mover el servo 3
    TRISDbits.TRISD9= 0;    //Establecer el pin RD9 como salida. Utilizado para mover el servo 4
}

//Inicializacion de pines de salida. Deberemos inicializarlos a 0 para que esten desactivados al principio
void inic_pwm() {
    LATDbits.LATD0 = 0;  //Inicializamos RD0 como salida de la se?al PWM del servomotor 1 a 0
    LATDbits.LATD1 = 0;  //Inicializamos RD1 como salida de la se?al PWM del servomotor 2 a 0
    LATDbits.LATD2 = 0;  //Inicializamos RD2 como salida de la se?al PWM del servomotor 3 a 0
    LATDbits.LATD3 = 0;  //Inicializamos RD3 como salida de la se?al PWM del servomotor 4 a 0
    LATDbits.LATD9 = 0;  //Inicializamos RD9 como salida de la se?al PWM del servomotor 5 a 0
}
