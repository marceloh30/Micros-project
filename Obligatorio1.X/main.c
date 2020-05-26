/*
 * File:   main.c
 * Authors: Julián Ferreira y Marcelo Hernandez
 *
 * Created on 15 de abril de 2020, 06:59 PM
 */

// Seteo de Bits de Configuración del PIC:
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

//Macros:
#define _XTAL_FREQ 4000000 //Frec. de Cristal que usamos (cristal de un NTSC color television receivers)
#define LARGO_ART 8         //Largo en EEPROM de cada artículo (2-TP, 6-Codigo)
#define LARGO_PRECIO 3      //Largo en EEPROM de precio de c/art.
#define CANT_ART 13         //Cantidad de artículos

#include <xc.h>
#include "variablesGlobales.h"
#include "mostrarInicializar.h"
#include "manejarProductos.h"
#include "lectura.h"
#include "acciones.h"
//Defino variables estaticas (debido a que cruzan funciones)
unsigned short int cuenta, auxCuenta;
short int huboInt = 0;
char serial = 0;
char modoDebug = 0;
short int productoIngresado;
short int numProd;
char codigoEntrada[10];
char ventasLote = 0;
unsigned short int montosLote = 0;
char nroLote = 1;
unsigned char prodIngresados[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0}; //99 lugares para 99 productos



void main(void) {
    
    //Seteo de entradas y salidas
    ADCON1 = 0b00000111;
    TRISA = 0x06; //Defino PORTA con RA1(Boton Aceptar) Y RA2(Boton deshacer) como entradas (botones) y el resto salidas (leds(2))
    TRISB = 0x00; //Defino PORTC como salidas (Unidades)
    TRISD = 0x00; //Defino PORTD como salidas (Decenas)
    INTCON = 0b11000000;    //Habilito las interupciones
    RCIE = 1;        //habilita interrupción por recepción.  
    iniciar_usart();        //inicia el módulo USART PIC (uso de puerto serial)
    cuenta = 0;
    auxCuenta = 0;
    mostrarDigitos(cuenta);
        
    //Bucle principal del programa
   
    while(1) {
        
        if(RA1) {
            while(RA1);
            accionesAceptar();
        }
        else if(RA2) {
            while(RA2);
            accionesDeshacer();
        }
        else if(huboInt) {
            huboInt = 0; 
            accionesPuertoSerial();
        }
    }
    
}

//rutina de atención a la interrupción por Rx serial
void __interrupt() int_usart() {
    
    if(RCIF == 1) {
        if(RCREG != 0x0D && RCREG != 0x0A && serial < 9) { //Verifico que no me sobrepase de los datos esperados! 
            codigoEntrada[serial] = RCREG;
            serial++;
        }
        else{
            serial = 0;
            huboInt = 1;
        }
    }
    
    
}