/*
 * Fecha: 09/05/2024
 * Autores: Iñaki Azpiroz, Gorka Dabo, Unai Cartagena
 */

// definiciones generales

#define Nop() __builtin_nop()

#define _ISR_NO_PSV __attribute__((interrupt, no_auto_psv))

/////////////////////////////////////
// definiciones para la pantalla LCD
/////////////////////////////////////

/*
For Explorer 16 board, here are the data and control signal definitions
RS -> RB15
E -> RD4
RW -> RD5
DATA -> RE0 - RE7
 */

// Control signal data pins
#define RW LATDbits.LATD5 // LCD R/W signal
#define RS LATBbits.LATB15 // LCD RS signal
#define E LATDbits.LATD4 // LCD E signal

// Control signal pin direction
#define RW_TRIS TRISDbits.TRISD5
#define RS_TRIS TRISBbits.TRISB15
#define E_TRIS TRISDbits.TRISD4

// Data signals and pin direction
#define DATA LATE // Port for LCD data
#define DATAPORT PORTE
#define TRISDATA TRISE // I/O setup for data Port


/////////////////////
// definiciones posiciones cronometro
//////////////////////
#define posds 13
#define posseg 10
#define posmin 7

/////////////////////
// Pines analógicos de entrada
//////////////////////
#define GATILLO 3  //RB3
#define TEMP 4     //RB4
#define POT 5      //RB5
#define X 8        //RB8
#define Y 9        //RB9
#define Z 10       //RB10



#define PR20ms 62500 //Numero de ciclos para que pasen 20ms con prescaler 1:8

//Definicion valores del Duty
#define DutyMax 6563    //Duty máximo
#define DutyMiddle 4063 //Duty medio
#define DutyMin 1562    //Duty mínimo
#define DutyBorder 50   //Valor en el que se incrementa o decrementa el valor del duty. Es utilizado cuando se mueve el brazo en modo digital.

//Posiciones de cada servo en el vector duties
#define servoX 0
#define servoY 1
#define servoZ 2
#define servoPinzaY 3
#define servoPinzaX 4

//Definicion posiciones del mensaje. Fila correspondiente de cada mensaje en la matriz Informacion
#define crono 3
#define tempYPot 4
#define joystickX 5
#define joystickY 6
#define joystickZ 7
#define PWM1 8
#define PWM2 9
#define PWM3 10
#define PWM4 11
#define PWM5 12


//Posicion de mensajes en vector Informacion. Utiles para saber a partir de que caracter debemos modificar el mensaje
#define pos_temp 2   
#define pos_pwm 8
#define posY 9
#define pos_pot 11
#define posJoystick 12
#define posZ 13

//Para acceder al servo correspondiente de cada eje dentro del vector duties
#define EJE_X 0
#define EJE_Y 1
#define EJE_Z 2

//Valor maximo medio y minimo del eje x para el joystick
#define JOY_X_MAX 895
#define JOY_X_MIN 58
#define JOY_X_IDLE 475

//Valor maximo medio y minimo del eje y para el joystick
#define JOY_Y_MAX 860
#define JOY_Y_MIN 0
#define JOY_Y_IDLE 485

//Valor maximo medio z minimo del eje y para el joystick
#define JOY_Z_MAX 1020
#define JOY_Z_MIN 15
#define JOY_Z_IDLE 500

//Zona en la cual el movimiento del joystick no afecta al movimiento del brazo robotico. Asi, en caso de movimiento leve del joystick, si la diferencia entre
//la poisicion actual y la posicion media no supera este valor, no se producirá el movimiento
#define ZONA_MUERTA 50


//Zona en la cual el movimiento de la palanca no afecta al movimiento del robot. Asi, en caso de movimiento leve de la palanca, si la diferencia entre
//la poisicion actual y la posicion media no supera este valor, no se producirá el movimiento. Valor escogido muy alto para que solamente se produzca el movimiento 
//en caso de mover hacia los extremos la palanca. Esto se debe al corto recorrido de la palanca.
#define ZONA_MUERTA_Z 325

//Valor en el que se modificará el duty del servo para cada iteración. Esto nos permite un control en el movimiento y una limitacion de velocidad del brazo robotico.
#define DESPLAZAMIENTO 6

//Valores de los duties de los servomotores de los ejes x, y, z a asignar para desplazar el brazo a la posición segura.
#define seg_X 4845
#define seg_Y 6563
#define seg_Z 6563

//Vector utilizado para almacenar de forma ordenada los valores de los flags.
extern int flags[6];

//Indices del vector flags, donde se define la posicion de cada uno de los flags.
#define flag_media 0    //Flag utilizado para llamar a la funcion CalcularMedia
#define flag_crono 1    //Flag utilizado para llamar a la funcion cronometro
#define flag_val_servo 2 //Flag utilizado para llamar a actualizarValServos
#define flag_control 3  //Flag utilizado para conmutar el control del brazo mediante entradas analogicas o digitales
#define flag_seg 4      //Flag utilizado para llamar al metodo posicionSegura. Este flag se activa con el pulsador RD13
#define flag_pinza 5    //Flag utilizado para controlar el funcionamiento del servomotor del eje X de la pinza

//Valores de las filas de la matriz muestras de cada entrada analógica. Es decir, en cada variable está guardado el valor
//de la fila en el que se guarda el valor muestreado y digitalizado de la entrada analogica correspondiente.
#define fila_pot 0
#define fila_temp 1
#define fila_X 2
#define fila_Y 3
#define fila_Z 4

//Variables globales utilizadas en el proyecto
extern int valorPOT, valorTEMP, valorADC, sel_servo, mediaPot;

//Vector con el duty de todos los servomotores
extern int duties[5];