/* Funciones para trabajar con el modulo UART2
================================================
Autores: Gorka Dabo, Unai Cartagena e I?aki Azpiroz 
Fecha: 09/05/2024
 */
#include "p24HJ256GP610A.h"
#include "timers.h"
#include "mensajes.h"
#include "commons.h"
// Definiciones ligadas a la velocidad de transmision de UART2

#define Fosc 50000000       // Frecuencia de reloj de la CPU (oscillator)
#define Fcy	 Fosc/2         // Velocidad de ejecucion de las instrucciones
#define BAUDRATE2 9600 		// Velocidad de transmision de UART2 (baudios)

#define T_1BIT_US (1000000/BAUDRATE2)+1 // Duracion de 1 bit en us

#define BAUD_RATEREG_2_BRGH1 ((Fcy/BAUDRATE2)/4)-1	// valor U2BRG si BRGH=1
#define BAUD_RATEREG_2_BRGH0 ((Fcy/BAUDRATE2)/16)-1	// valor U2BRG si BRGH=0

// Definiciones relacionadas con la pantalla del terminal del PC

#define clrscr "\x1b[2J" 	//4 character: \x1b, [, 2, J
#define home "\x1b[H"    	//3 character: \x1b, [, H

#define CR 0x0D		// carriage return
#define LF 0x0A		// line feed

void inic_UART2() {

    // Velocidad de transmision
    // Hay que hacer solo una de las dos asignaciones siguientes
    U2BRG = BAUD_RATEREG_2_BRGH1;
    //U2BRG = BAUD_RATEREG_2_BRGH0;

    // U2MODE: habilitar el modulo (UARTEN), 8 bits, paridad par (PDSEL),
    // 1 bit de stop (STSEL), BRGH ...
    U2MODE = 0;
    U2MODEbits.BRGH = 1;

    // U2STA: modo de interrupcion en el envio (UTXISEL), habilitacion del
    // envio (UTXEN), modo de interrupcion en la recepcion (URXISEL)
    U2STA = 0;

    // inicializacion de los bits IE e IF relacionados (IP si se quiere modificar)
    IFS1bits.U2RXIF = 0;
    IFS1bits.U2TXIF = 0;
    IEC1bits.U2RXIE = 1;
    IEC1bits.U2TXIE = 1;

    //IPC7bits.U2RXIP=xx;
    //IPC7bits.U2TXIP=xx;

    // interrupciones debidas a errores + bug
    IEC4bits.U2EIE = 0;
    U2STAbits.OERR = 0;

    // Habilitar el modulo y la linea TX.
    // Siempre al final y en ese orden!!!

    U2MODEbits.UARTEN = 1; // habilitar UART2
    U2STAbits.UTXEN = 1; // habilitar transmision tras habilitar modulo

    Delay_us(T_1BIT_US); // Esperar tiempo de 1 bit 
    //Llenar buffer U2TXREG para que la rutina de atencion U2TX salte por primera vez.
    U2TXREG = 0;

}

/** Rutina de atencion de la UART. Esta rutina de atencion salta cuando el buffer U2RXREG se llena. Esto ocurre cuando recibimos
 * un caracter por teclado. La rutina comprueba el caracter especifico pulsado, y realiza la accion correspondiente a ese caracter.   
 */
void _ISR_NO_PSV _U2RXInterrupt() {
    //Se guarda el valor del caracter en la variable 'tecla' 
    char tecla = U2RXREG;
    /** Comprobamos el caracter recibido. Para todos los casos admitimos el caracter tanto en mayuscula como en minuscula
     * Este es el esquema de los caracteres y sus acciones con respecto al cronometro:
     *  Si 'p' o 'P' --> Parar el cronometro
     *  Si 'c' o 'C' --> Reanudar el cronometro
     *  Si 'I' o 'i' --> Iniciar el cronometro
     *  Si 'A' o 'a' --> Mover brazo a la izquierda
     *  Si 'D' o 'd' --> Mover brazo a la derecha
     *  Si 'W' o 'w' --> Mover brazo hacia arriba
     *  Si 'S' o 's' --> Mover brazo hacia abajo
     *  Si 'T' o 't' --> Mover brazo hacia adelante
     *  Si 'G' o 'g' --> Mover brazo hacia detras
     *  Si 'U' o 'u' --> Mover servo de pinza hacia arriba
     *  Si 'J' o 'j' --> Mover servo de pinza hacia abajo
     *  Si 'K' o 'k' --> Cierra pinza 
     *  Si 'H' o 'h' --> Abre pinza
     **/
    
    switch (tecla) {
        case 'P':
            stop_Timer4(); // Parar el cron?metro
            break;
        case 'p':
            stop_Timer4(); // Parar el cron?metro
            break;
        case 'C':
            resume_Timer4(); // Continuar el cron?metro
            break;
        case 'c':
            resume_Timer4(); // Continuar el cron?metro
            break;
        case 'I':
            inic_crono(); //Iniciar el cronometro
            break;
        case 'i':
            inic_crono(); //Iniciar el cronometro
            break;
            
            
            //Mover el brazo en el eje X, hacia la derecha tecla D, hacia la izquierda tecla A.
        case 'D':
            if (flags[flag_control] == 1) {
                incrementar_servo(servoX); //Incrementar valor del servo para mover a la derecha 
            }
            break;
        case 'd':
            if (flags[flag_control] == 1) {
                incrementar_servo(servoX); //Incrementar valor del servo para mover a la derecha
            }
            break;
        case 'A':
            if (flags[flag_control] == 1) {
                decrementar_servo(servoX); //Decrementar valor del servo para mover a la izquierda
            }
            break;
        case 'a':
            if (flags[flag_control] == 1) {
                decrementar_servo(servoX); //Decrementar valor del servo para mover a la izquierda
            }
            break;

            //Mover el brazo en el eje Y, hacia la derecha tecla W, hacia la izquierda tecla S.
        case 'W':
            if (flags[flag_control] == 1) {
                incrementar_servo(servoY); //Incrementar valor del servo para mover arriba
            }
            break;
        case 'w':
            if (flags[flag_control] == 1) {
                incrementar_servo(servoY); //Incrementar valor del servo para mover arriba
            }
            break;
        case 'S':
            if (flags[flag_control] == 1) {
                decrementar_servo(servoY); //Decrementar valor del servo para mover abajo
            }
            break;
        case 's':
            if (flags[flag_control] == 1) {
                decrementar_servo(servoY); //Decrementar valor del servo para mover abajo
            }
            break;

            //Mover el brazo en el eje Z, hacia delante tecla T, hacia detr?s tecla G.
        case 'T':
            if (flags[flag_control] == 1) {
                incrementar_servo(servoZ); //Incrementar valor del servo para mover hacia delante
            }
            break;
        case 't':
            if (flags[flag_control] == 1) {
                incrementar_servo(servoZ); //Incrementar valor del servo para mover hacia delante
            }
            break;
        case 'G':
            if (flags[flag_control] == 1) {
                decrementar_servo(servoZ); //Decrementar valor del servo para mover hacia detr?s
            }
            break;
        case 'g':
            if (flags[flag_control] == 1) {
                decrementar_servo(servoZ); //Decrementar valor del servo para mover hacia detr?s
            }
            break;

            //Mover la pinza en el eje Y, hacia arriba U, hacia abajo tecla J.
        case 'U':
            if (flags[flag_control] == 1) {
                incrementar_servo(servoPinzaY); //Incrementar valor del servo para mover hacia arriba
            }
            break;
        case 'u':
            if (flags[flag_control] == 1) {
                incrementar_servo(servoPinzaY); //Incrementar valor del servo para mover hacia arriba
            }
            break;
        case 'J':
            if (flags[flag_control] == 1) {
                decrementar_servo(servoPinzaY); //Decrementar valor del servo para mover hacia abajo
            }
            break;
        case 'j':
            if (flags[flag_control] == 1) {
                decrementar_servo(servoPinzaY); //Decrementar valor del servo para mover hacia abajo
            }
            break;

            //Mover la pinza en el eje X, para cerrar pinza tecla H, abrir pinza tecla K.
        case 'K':
            if (flags[flag_control] == 1) {
                incrementar_servo(servoPinzaX); //Incrementar valor del servo para cerrar pinza
            }
            break;
        case 'k':
            if (flags[flag_control] == 1) {
                incrementar_servo(servoPinzaX); //Incrementar valor del servo para cerrar pinza
            }
            break;
        case 'H':
            if (flags[flag_control] == 1) {
                decrementar_servo(servoPinzaX); //Decrementar valor del servo para mover hacia derecha
            }
            break;
        case 'h':
            if (flags[flag_control] == 1) {
                decrementar_servo(servoPinzaX); //Decrementar valor del servo para mover hacia derecha
            }
            break;
    }

    IFS1bits.U2RXIF = 0;
}

//Variables utilizadas en rutina de atencion U2TX.
int indice = 0;   //Indice que el caracter actual del mensaje
int linea = 0;    //Sirve como indice de mensaje actual
int contHome = 0; //Indice para vector home, donde mandaremos los 3 caracteres que pondran el cursor al principio

/** Rutina de atencion de la UART. Esta rutina de atencion salta cuando el registro U2TXREG se llena. 
 *  En esta rutina actualizan los mensajes de la matriz Informacion y se envian por la linea TX de la UART. 
 *  
 */
void _ISR_NO_PSV _U2TXInterrupt() {
   //Iterar y actualizar todos los caracteres de un mensaje
    if (indice < 18) {
        U2TXREG = Informacion[linea][indice];
        indice++;
    }
    /**
     *Cuando todos los mensajes de la matriz han sido actualizados en pantalla. En ese caso se pondra el cursor
     *al inicio de la pantalla **/
    if (indice == 18 && linea == 12) {
        U2TXREG = home[contHome];
        contHome++;
        //Si ya se han mandado los caracteres para posicionar el cursor arriba a la derecha
        if (contHome == 3) {
            linea = -1; //Ponemos -1 para que luego haga +1 y asi que quede en 0, queriamos poner indice=0 pero si lo haciamos spameaba la pantalla
            contHome = 0;
        }
        //En caso de quedar mensajes de la matriz por actualizar
    } else if (indice == 18 && linea != 12) {
        indice = 0;
        linea = (linea + 1) % 13;
    }


    IFS1bits.U2TXIF = 0;
}