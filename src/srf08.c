/*
 * Fecha: 09/05/2024
 * Autores: Iñaki Azpiroz, Gorka Dabo, Unai Cartagena
 */


#include "i2c_funciones.h"
#define Leer_cm 0x51
#define Comando_escribir 0x00
#define Comando_leer 0x02
unsigned int inic_medicion_dis(unsigned char dirI2C)
// Puesta en marcha de una medicion
// Parametro dirI2C corresponde a la direccion I2C del sensor
{
   return LDByteWriteI2C_1(dirI2C, Comando_escribir, Leer_cm);
}

unsigned int leer_medicion(unsigned char dirI2C, unsigned char *dis)
// Funcion para leer la distancia medida
// Parametro dirI2C corresponde a la direccion I2C del sensor
// Parametro dis proporciona la direccion de un array en el que devolver la medicion
// Cada medicion 2 bytes
{
    return LDByteReadI2C_1(dirI2C, Comando_leer,dis, 2);
}

void cambiar_direccion(unsigned char dirI2C, unsigned char newdirI2C)
// Fucnion para cambiar la direccion I2C de un sensor
// Parametro dirI2C corresponde a la direccion I2C del sensor
// Parametro newdirI2C corresponde a la nueva direccion I2C que se le va a dar
{
    // A completar
}

unsigned int detectar_direccion(unsigned char *dirI2C)
// Funcion para detectar la direccion I2C del sensor conectado
// Si el sensor responde, la funcion devuelve 0 y actualiza el parametro dirI2C
// Si no responde ningun sensor, devuelve 1
{
    // A completar
    return 0;
}


