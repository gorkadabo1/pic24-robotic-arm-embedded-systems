/*
 * Fecha: 09/05/2024
 * Autores: Iñaki Azpiroz, Gorka Dabo, Unai Cartagena
 */

#include "p24HJ256GP610A.h"
#include "commons.h"
#include "LCD.h"
#include "mensajes.h"
#include "timers.h"
#include "utilidades.h"


//Funcion de inicialización del modulo ADC

void inic_ADC1(void) {
    // Inicializacion registro control AD1CON1
    AD1CON1 = 0; // todos los campos a 0

    // Comienzo digitalizacion automatico
    // 111=Auto-convert / 010=TMR3 ADC1 y TMR5 ADC2 / 001=INT0 / 000= SAMP
    AD1CON1bits.SSRC = 7;

    // Inicializacion registro control AD1CON2
    AD1CON2 = 0; // todos los campos a 0


    // Inicializacion registro control AD1CON3
    AD1CON3 = 0; // todos los campos a 0

    //Con los siguientes calculos de SAMC y ADCS, se harán 2000 conversiones (muestreo + digitalizacion) por cada segundo; por lo que se llamara a la rutina de atencion 2000 veces por segundo
    AD1CON3bits.SAMC = 31; // Tiempo muestreo = numero de Tad
    AD1CON3bits.ADCS = 41; // Relacion entre TAD y Tcy TAD = Tcy(ADCS+1). 

    // Inicializacion registro control AD1CON4
    AD1CON4 = 0;

    // Inicializacion registro AD1CHS123
    AD1CHS123 = 0; //seleccion del canal 1,2 eta 3

    // Inicializacion registro AD1CHS0
    AD1CHS0 = 0;
    AD1CHS0bits.CH0SA = POT; // elige la entrada analogica conectada. Potenciometro

    // Inicializacion registros AD1CSS
    // Si escaneo secuencial 1, si no 0
    AD1CSSH = 0; // 16-31
    AD1CSSL = 0; // 0-15

    // Inicializacion registros AD1PCFG. Inicialmente todas AN como digitales
    AD1PCFGH = 0xFFFF; // 1= digital / 0= Analog
    AD1PCFGL = 0xFFFF; // Puerto B, todos digitales

    // Inicializar como entradas analogicas solo las que vayamos a usar
    AD1PCFGLbits.PCFG5 = 0; // potenciometro
    AD1PCFGLbits.PCFG4 = 0; // sensor temperatura
    AD1PCFGLbits.PCFG8 = 0; //sensor ejeX
    AD1PCFGLbits.PCFG9 = 0; //sensor ejeY
    AD1PCFGLbits.PCFG10 = 0; //sensor ejeZ

    // Bits y campos relacionados con las interrupciones
    IFS0bits.AD1IF = 0;
    IEC0bits.AD1IE = 1;

    //AD1CON
    AD1CON1bits.ADON = 1; // Habilitar el modulo ADC
    AD1CON1bits.SAMP = 1;
}

//Matriz donde guardamos las muestras recogidas con el módulo ADC. Las 5 filas de la matriz son valores de: Potenciometro, sensor de temperatura, Eje X Eje Y Eje Z del joystick
unsigned int muestras[5][8];


/* Variable que determina el numero de muestras recogidas por cada muestra que tenemos que tomar. Es decir, una vez hayamos tomado una muestra
 * de todas las entradas que tenemos, el inidice aumentará su valor en 1 hasta que lleguemos a 8 muestras por cada entrada; en ese momento, 
 * reiniciará su valor.
 */
int indice_ADC = 0;

//Variables locales utilizadas para recoger el valor del buffer.
int valorTEMP, valorPOT, valorX, valorY, valorZ;

/* Rutina de atención del módulo ADC. La manera de funcionar es la siguiente: Primero, se guarda el valor del buffer ADC1BUF0 tomado del 
 * potenciómetro en una variable, y se almacena en su fila correspondiente en la matriz de muestras. Tras ello, se cambia el canal al de 
 * la siguiente entrada analógica escogida, y se sigue este mismo proceso hasta llegar al canal de la quinta entrada analógica, que en nuestro
 * caso se trata del valor de la palanaca del joystick (eje Z). En este último caso, se actualiza el valor del índice. Una vez completado el ciclo
 * de lectura de todos los canales (POT, TEMP, X, Y, Z), se incrementa el índice de muestras y se verifica si se han llenado todas las muestras
 * necesarias para el total de 8 muestras. Si se ha llegado a este limite, el índice se reinicia a 0 y se activa un flag para indicar
 * que se deben calcular las medias de las muestras tomadas. En el bucle del programa principal, se comprueba el valor de este flag constantemente, 
 * para que en el caso de que se active, se llame a la función que se encarga de calcular la media de las muestras tomadas. Finalmente, 
 * se reinicia la muestra del ADC estableciendo el bit SAMP a 1, y se limpia el flag de interrupción AD1IF para permitir que nuevas 
 * interrupciones del ADC ocurran. Esta lógica implementada permite una captura continua y ordenada de datos analógicos para su posterior 
 * procesamiento en el main.
 */
void _ISR_NO_PSV _ADC1Interrupt() {
    if (AD1CHS0bits.CH0SA == POT) {
        valorPOT = ADC1BUF0;
        muestras[fila_pot][indice_ADC] = valorPOT;
        AD1CHS0bits.CH0SA = TEMP;
    } else if (AD1CHS0bits.CH0SA == TEMP) {
        valorTEMP = ADC1BUF0;
        muestras[fila_temp][indice_ADC] = valorTEMP;
        AD1CHS0bits.CH0SA = X;
    } else if (AD1CHS0bits.CH0SA == X) {
        valorX = ADC1BUF0;
        muestras[fila_X][indice_ADC] = valorX;
        AD1CHS0bits.CH0SA = Y;
    } else if (AD1CHS0bits.CH0SA == Y) {
        valorY = ADC1BUF0;
        muestras[fila_Y][indice_ADC] = valorY;
        AD1CHS0bits.CH0SA = Z;
    } else if (AD1CHS0bits.CH0SA == Z) {
        valorZ = ADC1BUF0;
        muestras[fila_Z][indice_ADC] = valorZ;
        indice_ADC++;
        AD1CHS0bits.CH0SA = POT;
    }

    if (indice_ADC == 8) {
        indice_ADC = 0;
        flags[flag_media] = 1;
    }
    AD1CON1bits.SAMP = 1;
    IFS0bits.AD1IF = 0;
}


/**
 * Esta función es la encargada de modificar el valor del duty del servo correspondiente al eje X, a partir del valor medio calculado en la 
 * función que se encarga de calcular la media de las 8 muestras tomdas. Para ello, define una variable inicializado con valor 0. Luego, se
 * definen 5 campos dentro del eje X. A partir del valor que tiene el joystick en reposo en el eje X, hemos definido una zona al rededor de 
 * ese valor, en el que el movimiento del joystick no afecta al valor del servomotor correspondiente. Esta zona, tiene el valor 50, por lo que 
 * si el joystick se mueve entre [valor medio - zona muerta, valor medio + zona muerta]; no se moverá el brazo robótico en el eje X. Si se supera
 * este umbral de no actividad, existen dos posibilidades: Que el joystick se encuentre a la izquierda del valor medio, o que se encuentre a la 
 * derecha del valor medio. Independientemente en cual de las dos partes esté, en ambas partes hemos definido dos zonas, en las que el movimiento
 * se hace a distintas velocidades. Por un lado, si el joystick se encuentra en la mitad mas cercana al centro, el movimiento se hace a velocidad
 * desplazamiento/2, y si se encunetra en la mitad más lejana, el movimiento se hace a velocidad de desplazamiento. Además, después de calcular
 * la modificación que se realizará al duty, se comprueba a ver si el resultado supera alguno de los limites, para que en el caso de que así sea, 
 * se recalcule el desplazamiento de tal manera que que el duty resultante justo quede en el limite sin superarlo. En cuanto al calculo de la
 * modificación que se realiza al duty, en el caso de que se desplze el joystick a la izquierda, la variable tomará un valor negativo, para que
 * cuando al final de la funcion se llame a la función encargada de modificar el duty, al valor sin modificar del duty se le suma ese valor, para 
 * que resulte en una resta, y si el joystick se mueve hacia la derecha, la variable tomará un valor positivo, que al sumarse al valor del duty,
 * aumentará su valor. Al final de la función, se actualiza el valor del mensaje que contiene la informacion acerca de la posicion en la que se
 * encuentra el joystick.
 */
int coord_X;

void aumentarX(unsigned int x) {
    coord_X = 0;
    if (x < JOY_X_IDLE - ZONA_MUERTA) {
        if (x < (JOY_X_IDLE - JOY_X_MIN) / 2) {
            coord_X -= DESPLAZAMIENTO;
        } else {
            coord_X -= DESPLAZAMIENTO / 2;
        }
        if (duties[EJE_X] - coord_X < DutyMin) coord_X = DutyMin - duties[EJE_X];
    } else if (x > JOY_X_IDLE + ZONA_MUERTA) {
        if (x > (JOY_X_MAX - JOY_X_IDLE) / 2) {
            coord_X += DESPLAZAMIENTO;
        } else {
            coord_X += DESPLAZAMIENTO / 2;
        }
        if (duties[EJE_X] + coord_X > DutyMax) coord_X = DutyMax - duties[EJE_X];
    }
    changeDuties(EJE_X, duties[EJE_X] + coord_X);
    conversion(&Informacion[joystickX][posJoystick], x);
}


/**
 * Esta funcion es la encargada de modificar el valor del duty del servo correspondiente al eje Y, a partir del valor medio calculado en la 
 * funcion que se encarga de calcular la media de las 8 muestras tomadas. El funcionamiento de la funcion, es exactamente la misma que la de la
 * funcion que se encarga de gestionar el eje X, con la diferencia de que se utilizan los valores medio, maximo, minimo y duty correspondientes 
 * al eje Y.
 */
int coord_Y;
void aumentarY(unsigned int y) {
    coord_Y = 0;
    if (y < JOY_Y_IDLE - ZONA_MUERTA) {
        if (y < (JOY_Y_IDLE - JOY_Y_MIN) / 2) {
            coord_Y -= DESPLAZAMIENTO;
        } else {
            coord_Y -= DESPLAZAMIENTO / 2;
        }
        if (duties[EJE_Y] - coord_Y < DutyMin) coord_Y = DutyMin - duties[EJE_Y];
    } else if (y > JOY_Y_IDLE + ZONA_MUERTA) {
        if (y > (JOY_Y_MAX - JOY_Y_IDLE) / 2) {
            coord_Y += DESPLAZAMIENTO;
        } else {
            coord_Y += DESPLAZAMIENTO / 2;
        }
        if (duties[EJE_Y] + coord_Y > DutyMax) coord_Y = DutyMax - duties[EJE_Y];
    }
    changeDuties(EJE_Y, duties[EJE_Y] + coord_Y);
    conversion(&Informacion[joystickY][posJoystick], y);
}

/**
 * Esta funcion es la encargada de modificar el valor del duty del servo correspondiente al eje Z, a partir del valor medio calculado en la 
 * funcion que se encarga de calcular la media de las 8 muestras tomdas. El funcionamiento de la funcion, es exactamente la misma que la de la
 * funcion que se encarga de gestionar el eje X, con la diferencia de que se utilizan los valores medio, maximo, minimo y duty correspondientes 
 * al eje Y. Cabe destacar, que la zona muerta que hemos definido para el eje Z es mayor que el utilizado para los eje X e Y, ya que como el eje
 * Z tiene un recorrido muy reducido, no queriamos que con un mínimo moviemento se moviese el brazo, debido a que hemos considerado que un
 * movimiento asi podria darse por accidente.
 */
int coord_Z;
void aumentarZ(unsigned int z) {
    coord_Z = 0;
    if (z < JOY_Z_IDLE - ZONA_MUERTA_Z) {
        if (z < (JOY_Z_IDLE - JOY_Z_MIN) / 2) {
            coord_Z -= DESPLAZAMIENTO;
        } else {
            coord_Z -= DESPLAZAMIENTO / 2;
        }
        if (duties[EJE_Z] - coord_Z < DutyMin) coord_Z = DutyMin - duties[EJE_Z];
    } else if (z > JOY_Z_IDLE + ZONA_MUERTA_Z) {
        if (z > (JOY_Z_MAX - JOY_Z_IDLE) / 2) {
            coord_Z += DESPLAZAMIENTO;
        } else {
            coord_Z += DESPLAZAMIENTO / 2;
        }
        if (duties[EJE_Z] + coord_Z > DutyMax) coord_Z = DutyMax - duties[EJE_Z];
    }

    changeDuties(EJE_Z, duties[EJE_Z] + coord_Z);
    conversion(&Informacion[joystickZ][posJoystick], z);
}

/**
 * Esta función es la encargada de calcular el valor medio de las muestras recogidas para cada entrada analógica (potenciómetro, temperatura, 
 * y ejes X, Y, Z del joystick). Para ello, definimos varias variables en las que se almacena la suma total de los valores calculados, para luego 
 * almacenar el valor de la division de cada uno de estos valores entre 8, en otras nuevas variables. Luego, tanto para la temperatura como para
 * el potenciometro, se actualizan los mensajes correspondientes de la matriz con estos nuevos valores. En el caso de ejes X, Y, Z del joystick
 * primero se comprueba que el valor del flag de control sea 0 para asegurarnos de que se quiere mover el brazo robotico mediante las entradas
 * analogicas y no mediante digitales (UART), y tambien se comprueba que el valor del falg de seguridad sea 0, ya que no queremos que mientras 
 * el brazo este en proceso de colocarse en una posicion segura, se interrumpa el proceso mediante los movimientos del joystick. Si estas
 * condicines se cumplen, se llama a las funciones encargadas de modificar los duties de cada uno de los ejes, pasando como parametro, el valor
 * medio calculado para cada eje. Para finalizar, se pone el flag media a 0, para indicar al main de que ya se ha terminado de calcular las medias.
 */
void calcularMedia() {
    unsigned int sumaPot, sumaTemp, sumaX, sumaY, sumaZ, mediaTemp, mediaX, mediaY, mediaZ, i;
    sumaTemp = 0;
    sumaPot = 0;
    sumaX = 0;
    sumaY = 0;
    sumaZ = 0;

    for (i = 0; i < 8; i++) {
        sumaPot += muestras[0][i];
        sumaTemp += muestras[1][i];
        sumaX += muestras[2][i];
        sumaY += muestras[3][i];
        sumaZ += muestras[4][i];
    }

    mediaPot = sumaPot / 8;
    mediaTemp = sumaTemp / 8;
    mediaX = sumaX / 8;
    mediaY = sumaY / 8;
    mediaZ = sumaZ / 8;

    conversion(&Informacion[tempYPot][pos_temp], mediaTemp);
    conversion(&Informacion[tempYPot][pos_pot], mediaPot);

    if (flags[flag_control] == 0 && flags[flag_seg] == 0) {
        aumentarX(mediaX);
        aumentarY(mediaY);
        aumentarZ(mediaZ);
    }

    flags[flag_media] = 0;
}