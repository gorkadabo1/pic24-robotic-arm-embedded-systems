/*
 * Fecha: 09/05/2024
 * Autores: Iñaki Azpiroz, Gorka Dabo, Unai Cartagena
 */

#include "p24HJ256GP610A.h"
#include "LCD.h"
#include "commons.h"
#include "timers.h"
#include "utilidades.h"

//Matriz donde se almacenan todos los mensajes que mostraremos en pantalla LCD y a través de la linea TX del UART. Cada mensaje es una fila.
unsigned char Informacion[13][18] = {
    "Proyecto Brazo R""\x0D\x0A",
    "PushS3 to scroll""\x0D\x0A",
    "S3 st/stop, S6=0""\x0D\x0A",
    "Crono: --:--,-  ""\x0D\x0A",
    "T:----  P: ---- ""\x0D\x0A",
    "JOYSTICK X: ----""\x0D\x0A",
    "JOYSTICK Y: ----""\x0D\x0A",
    "JOYSTICK Z: ----""\x0D\x0A",
    "PWM S1: ----    ""\x0D\x0A",
    "PWM S2: ----    ""\x0D\x0A",
    "PWM S3: ----    ""\x0D\x0A",
    "PWM S4: ----    ""\x0D\x0A",
    "PWM S5: ----    ""\x0D\x0A"
};

//Variable utilizada como contador, para saber los mensajes que hay que mostrar en la LCD y el refresco dee los mensajes realizados
int fila = 0;
//Este metodo selecciona el siguiente mensaje de la matriz. Permite el scroll en la pantalla LCD junto al metodo actualizarPantalla().
void scrollear() {
    fila = (fila + 1) % 12;
}

//Este metodo actualiza la pantalla de la LCD. Muestra el mensaje en la posicion 'fila' y el siguiente en la matriz
void actualizarPantalla() {
    line_1();
    puts_lcd(Informacion[fila], 18);
    line_2();
    puts_lcd(Informacion[fila + 1], 18);

}

//Este metodo actualiza el valor del servo de la posicion 'sel_servo' en el vector duties.
void actualizarValorServos() {
    //Actualizar visualizacion del valor del servomotor especifico
    conversion(&Informacion[sel_servo + 8][pos_pwm], duties[sel_servo]);
    //Desactivar flag
    flags[flag_val_servo] = 0;
}