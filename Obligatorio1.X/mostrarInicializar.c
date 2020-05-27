#include <xc.h>
#include "variablesGlobales.h"

static unsigned const char BMS[] = {
    0b00000000,
    0b00010000,
    0b00100000,
    0b00110000,
    0b01000000,
    0b01010000,
    0b01100000,
    0b01110000,
    0b10000000,
    0b10010000,
};

void mostrarDigitos(unsigned int num) { //num debe ser el numero entero (por ej. 99,1 --> 99)
    //Verifico si puedo redondear el numero 
    PORTB = BMS[num/100]; // Tomo las decenas
    PORTB = PORTB | ((num%100)/10); // Tomo la unidad
    PORTD = BMS[(num%100)%10]; // Tomo la decima
     
}

void iniciar_usart(){//función para iniciar el módulo USART PIC
     TRISC = 0b10000000;//pin RX como una entrada digital pin TX como una salida digital
     TXSTA = 0b00100110;// 8bits, transmisión habilitada, asíncrono, alta velocidad
     RCSTA = 0b10010000;//habilitado el USART PIC, recepción 8 bits,  habilitada, asíncrono
     SPBRG = 25; //Valor  para una velocidad de 9600 baudios con un oscilador de 4 Mhz 
}

void bailenLeds() { 
    //Secuecnia de leds
    unsigned short int i;
    for (i = 0; i < 3; i++) {
        RA3 = 1;
        __delay_ms(200);
        RA5 = 1;
        __delay_ms(200);
        RA3 = 0;
        __delay_ms(200);
        RA5 = 0;
    }
}


void analogicoDigital(){
    
    
    
    
}


void envioTX(char *mensaje){
    int i;
    while(i < strlen(mensaje)){
        TXIE = 1;
        TXREG = mensaje[i];
        i++
    }
}