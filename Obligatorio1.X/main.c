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

#define _XTAL_FREQ 3579545 //Frec. de Cristal que usamos (cristal de un NTSC color television receivers)
#define LARGO_ART 8         //Largo en EEPROM de cada artículo (2-TP, 6-Codigo)
#define LARGO_PRECIO 3      //Largo en EEPROM de precio de c/art.
#define CANT_ART 15         //Cantidad de artículos

#include <xc.h>
#include <string.h>


unsigned short int cuenta, auxCuenta;
static short int huboInt = 0;
const char codigoEntrada[9]; //Le puse const porque cruzaba funciones (no se si me explique..)

unsigned const char digito[] = {
    0b11111100, //digitos en binario del 0 al 9 (cátodo común)
    0b01100000,     //1...
    0b11011010,
    0b11110010,
    0b01100110,
    0b10110110,
    0b10111110,
    0b11100010,
    0b11111110,     //8
    0b11100110 };   //9.

unsigned const char *productos[] =  {"012020016", "012020027",
                                    "022020017","022020028","022020039",
                                    "032020018","032020029",
                                    "042020019","042020020","042020031",
                                    "052020010","052020011","052020012"};
unsigned const char *precios[] =    {"010", "025",
                                    "007","011","015",
                                    "012","023",
                                    "016","020","023",
                                    "041","053","065"};

void mostrarDigitos(unsigned int num) { //num debe ser el numero entero (por ej. 99,1 --> 99)
    unsigned short int decimal;
    //Verifico si puedo redondear el numero
    if (num/10 < 99) {
        decimal = num%10;
        num =/10;
        if (num%10 >= 5) {
            num++;
        }
    }
    else { 
        num =/10;
    }
    
    //Obtengo cociente y resto del numero dividido entre 10 para decenas y unidades: 
    PORTB=digito[(num/10)];
    PORTC=digito[(num%10)];
    
}

void configuracionTimer(){
    OPTION_REG = 0b0001000; //Configuracion del timer, aumenta el timer en una unidad cada 1us
}

void iniciar_usart(){//función para iniciar el módulo USART PIC
     TRISCbits.TRISC7=1;//pin RX como una entrada digital
     TRISCbits.TRISC6=0;//pin TX como una salida digital
     TXSTA=0b00100110;// 8bits, transmisión habilitada, asíncrono, alta velocidad
     RCSTA=0b10010000;//habilitado el USART PIC, recepción 8 bits,  habilitada, asíncrono
     SPBRG=25;//para una velocidad de 9600 baudios con un oscilador de 4Mhz
    }

void accionesAceptar(){
    cuenta = 0;
    auxCuenta = 0;
    
    mostrarDigitos(cuenta);
    bailenLeds();
}

void accionesDeshacer(){
    if (cuenta != auxCuenta){
        cuenta = auxCuenta;
        mostrarDigitos(cuenta);
    }
}

void bailenLeds(){
    unsigned short int i;
    for (i = 0; i < 10; i++){
        RA3 = 1;
        __delay_ms(100);
        RA4 = 1;
        __delay_ms(100);
        RA3 = 0;
        __delay_ms(100);
        RA4 = 0;
    }
}

short int EEPROM_search(char *codigoRec) { //Puede ser la otra variable y no precisar parametro pero como quieras juli
    
    //Defino variables para la función
    short int esta = 0;
    short int direccion = 0;
    short int numProd = 0;
    short int precio;
    
    //Busco el código coincida con el código que recibí:
    while (esta < 8 || direccion < 96){
        for(int i = 0; (i < LARGO_ART) && (esta != 0); i++){
            if(codigoRec[i] == eeprom_read(direccion)){
                esta++;
            }
            else{
                esta = 0;
            }
            direccion++;
        }
        numProd++;
    }
    if (esta == 8){
        precio = eeprom_read(96 + numProd*8); 
    }
    else{
        precio = -1;
    }
    return precio;
}

void accionesPuertoSerial(){
    short int Aux = 0;
    char *stringCode;
    
    for (int i = 2; i < LARGO_ART; i++ ){
        Aux += codigoEntrada[i];
    }
    
    if(Aux == codigoEntrada[8]){
        codigoEntrada[8] = 0; //Si funciona en ascii
        stringCode = &codigoEntrada;
        EEPROM_search(stringCode);
    }
}
void main(void) {
    
    //Seteo de entradas y salidas
    TRISA = 0x06; //Defino PORTA con RA1(Boton Aceptar) Y RA2(Boton deshacer) como entradas (botones) y el resto salidas (leds(2))
    TRISB = 0x00; //Defino PORTC como salidas (Unidades)
    TRISD = 0x00; //Defino PORTD como salidas (Decenas)
    INTCON = 0b11000000; //Habilito las interupciones
    PIE1bits.RCIE=1;//habilita interrupción por recepción.
    
    iniciar_usart();//inicia el módulo USART PIC 
              
    
    if (eeprom_read(0x00) == 0xFF) {
        
        eeprom_write(0x00,productos);  
 
        
    }
    
    while(1) {
        
        if(RA1 == 1){
            accionesAceptar();
        }
        else if(RA2 == 2){
            accionesDeshacer();
        }
        else if(huboInt){
            accionesPuertoSerial();
        }
        
        
        
        
    }
    
    
    return;
}

void interrupt int_usart(){//rutina de atención a la interrupción por recepción
    int i = 0;
    int recibir = 1;
    huboInt = 1;
    while(recibir){
        if(RCIF == 1){
            if(RCREG != 0x0D || RCREG != 0x0A || i < 9){
                codigoEntrada[i] = RCREG;
                i++;
            }
            else{
                recibir = 0;
            }
        }
    }
}