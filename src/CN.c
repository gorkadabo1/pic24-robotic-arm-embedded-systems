/*
 * Fecha: 09/05/2024
 * Autores: Iñaki Azpiroz, Gorka Dabo, Unai Cartagena
 */

#include "p24HJ256GP610A.h"
#include "commons.h"
#include "timers.h"
#include "utilidades.h"
#include "mensajes.h"

//Metodo para inicializar los modulos de CN utilizados
void inic_CN() {
    CNEN1bits.CN4IE = 1;  // habilitacion de la interrupcion del pin CN4
    CNEN1bits.CN15IE = 1; // habilitacion de la interrupcion del pin CN15
    CNEN2bits.CN16IE = 1; // habilitacion de la interrupcion del pin CN16
    CNEN2bits.CN19IE = 1; // habilitacion de la interrupcion del pip CN19
    IEC1bits.CNIE = 1; // habilitacion de la interrupcion general de CN
    IFS1bits.CNIF = 0; // Puesta a 0 del flag IF del modulo
}


// RUTINA DE ATENCION A LA INTERRUPCION DE CN
void _ISR_NO_PSV _CNInterrupt() {
    //Comprueba si se ha pulsado el boton S3. En caso de haberlo pulsado, llama a la funcion scrollear, que realizara el scroll en la LCD
    if (!PORTDbits.RD6) {
        scrollear();
    }
    
    //Comprobacion de si se ha pulsado el gatillo del joystick. En caso de pulsarse, activa el flag que generara el cierre de la pinza del brazo. 
    //El valor de este flag, se utiliza en el main, para que cuando llama a la funcion encargada de mover la pinza, sepa si tiene que abrir o 
    //cerrar la pinza.
    if (!PORTBbits.RB2) {
        flags[flag_pinza] = 1;
    } else {
        flags[flag_pinza] = 0;
    }
    
    //Comprobacion de si se ha pulsado el boton S4. En ese caso, se llevara al brazo a la posicion segura
    if (!PORTDbits.RD13) {
        flags[flag_seg]=1;
    }
    
    //Comprobacion de si se ha pulsado el boton S6. En ese caso, se cambiara el modo de control del robot. Si estaba en analogico, pasa a digital, y viceversa.
    if (!PORTDbits.RD7) {
        changeMode();
    }
    
    IFS1bits.CNIF = 0;
}