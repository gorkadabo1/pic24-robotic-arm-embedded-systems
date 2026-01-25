/*
 * Fecha: 09/05/2024
 * Autores: Iñaki Azpiroz, Gorka Dabo, Unai Cartagena
 */

// Declaraciones de las funciones implementadas en srf08.c
unsigned int inic_medicion_dis (unsigned char dirI2C);
unsigned int leer_medicion (unsigned char dirI2C, unsigned char *dis) ;
void cambiar_direccion (unsigned char dirI2C, unsigned char newdirI2C);
unsigned int detectar_direccion (unsigned char *dirI2C);

