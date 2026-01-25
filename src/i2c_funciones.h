/*
 * Fecha: 09/05/2024
 * Autores: Iñaki Azpiroz, Gorka Dabo, Unai Cartagena
 */

//Declaraciones de las funciones implementadas en i2c_funciones.c
void InitI2C_1();
unsigned int LDByteReadI2C_1(unsigned char I2C_address, unsigned char Register_address, unsigned char *Data, unsigned char Length);
unsigned int LDByteWriteI2C_1(unsigned char I2C_address, unsigned char Register_address, unsigned char data);