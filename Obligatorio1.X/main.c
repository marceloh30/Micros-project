/*
 * File:   main.c
 * Author: YULI
 *
 * Created on 15 de abril de 2020, 06:59 PM
 */

#define _XTAL_FREQ 4000000  //Frecuencia: 4MHz 
#define LARGO_ART 8         //Largo en EEPROM de cada artículo (2-TP, 6-Codigo)
#define LARGO_PRECIO 3      //Largo en EEPROM de precio de c/art.
#define CANT_ART 15         //Cantidad de artículos

#include <xc.h>


unsigned short int cuenta, auxCuenta;

unsigned const char digito[] = {
    0b00000011, //digitos en binario del 0 al 9 (ánodo común)
    0b10011111,     //1...
    0b00100101,
    0b00001101,
    0b10011001,
    0b01001001,
    0b01000001,
    0b00011101,
    0b00000001,     //8
    0b00011001 };   //9.

unsigned const char *productos[] = {"01202001", "01202002"};
unsigned const char *precios[] = {"010", "025"};

void mostrarDigitos(unsigned int num) { //num debe ser el numero entero (por ej. 99,1 --> 99)
    
    //Obtengo cociente y resto del numero dividido entre 10 para decenas y unidades: 
    PORTB=digito[(num/10)];
    PORTC=digito[(num%10)];
    
}

void main(void) {
    
    //Seteo de entradas y salidas
    TRISA = 0x06; //Defino PORTA con RA1 Y RA2 como entradas (botones) y el resto salidas (leds(2))
    TRISB = 0x00; //Defino PORTC como salidas (Unidades)
    TRISD = 0x00; //Defino PORTD como salidas (Decenas)
    

    
              
    
    if (eeprom_read(0x00) == 0xFF) {
        
        eeprom_write(0x00,productos);  
 
        
    }
    
    
    while(1) {
        
        
    }
    
    
    return;
}
