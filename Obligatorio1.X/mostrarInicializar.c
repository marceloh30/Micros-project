#include "mostrarInicializar.h"

//Variables
static unsigned const char BS[] = {//BS=Binario a conversor bcd a 7 segementos 
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
    PORTB = BS[num/100]; // Tomo las decenas
    PORTB = PORTB | ((num%100)/10); // Tomo la unidad
    PORTD = BS[(num%100)%10]; // Tomo la decima
     
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

void envioTX(char *mensaje){
    int i = 0;
    char transmitir=1;
	while(transmitir){
		TXREG = mensaje[i];
		if (TXREG==0){
			transmitir=0;
        }
        else{
			i++;
        }
		while(TXIF==0); //me quedo esperando a que termine el byte
	}
    //Envio salto de linea
    TXREG = 13; 
    while(TXIF == 0);

}
