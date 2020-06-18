#ifndef MAIN_H
#define MAIN_H

// Seteo de Bits de Configuracion del PIC:
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

//Librerias
#include <xc.h>
#include "variablesGlobales.h"
#include "mostrarInicializar.h"
#include "manejoProductos.h"
#include "lectura.h"
#include "acciones.h"

//Redefino variables extern creadas en variablesGlobales.h
unsigned short int cuenta, auxCuenta;
short int huboInt = 0;
char serial = 0;
char modoDebug = 0;
short int productoIngresado;
short int numProd;
char codigoEntrada[10];
unsigned char ventasLote = 0;
unsigned short int montosLote = 0;
unsigned char nroLote = 1;
char cierreLotePedido = 0;
unsigned char prodIngresados[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0}; //99 lugares para 99 productos (1 en c/bit)
unsigned int adresult = 0;
unsigned char pedidoVoltaje = 0;

//Firmas:
void main(void);
void __interrupt() int_usart(void);

#endif /* MAIN_H */
