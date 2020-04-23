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
#define _XTAL_FREQ 3579545 //Frec. de Cristal que usamos (cristal de un NTSC color television receivers)
#define LARGO_ART 8         //Largo en EEPROM de cada artículo (2-TP, 6-Codigo)
#define LARGO_PRECIO 3      //Largo en EEPROM de precio de c/art.
#define CANT_ART 13         //Cantidad de artículos

#include <xc.h>
#include <string.h>
#include <math.h>

//Defino variables estaticas (debido a que cruzan funciones)
static unsigned short int cuenta, auxCuenta;
static short int huboInt = 0;
static char codigoEntrada[9]; 

static unsigned const char digito[] = {
    0b11111100, //digitos en binario del 0 al 9 (catodo comun)
    0b01100000,     //1...
    0b11011010,
    0b11110010,
    0b01100110,
    0b10110110,
    0b10111110,
    0b11100010,
    0b11111110,     //8
    0b11100110 };   //9.

//Productos: Cada producto tiene TP+Codigo+Precio+byte Nulo, cada lugar corresponde a un producto!!
static unsigned const char *productos[] =  {(unsigned const char *)"01202001010", (unsigned const char *)"01202002025",
                                    (unsigned const char *)"02202001007",(unsigned const char *)"02202002011",(unsigned const char *)"02202003015",
                                    (unsigned const char *)"03202001012",(unsigned const char *)"03202002023",
                                    (unsigned const char *)"04202001016",(unsigned const char *)"04202002020",(unsigned const char *)"04202003023",
                                    (unsigned const char *)"05202001041",(unsigned const char *)"05202001053",(unsigned const char *)"05202001065"};
static unsigned short int prodIngresados = 0b0000000000000000; //16 lugares para 16 productos (extendible)
//Agrego con un or y verifico con un and!


void mostrarDigitos(unsigned int num) { //num debe ser el numero entero (por ej. 99,1 --> 99)
    unsigned short int decimal;
    //Verifico si puedo redondear el numero
    if (num/10 < 99) {
        decimal = num%10;
        num = num/10;
        if (num%10 >= 5) {
            num++;
        }
    }
    else { 
        num = num/10;
    }
    
    //Obtengo cociente y resto del numero dividido entre 10 para decenas y unidades: 
    PORTB=digito[(num/10)];
    PORTD=digito[(num%10)];
    
}

void iniciar_usart(){//función para iniciar el módulo USART PIC
     TRISCbits.TRISC7=1;//pin RX como una entrada digital
     TRISCbits.TRISC6=0;//pin TX como una salida digital
     TXSTA=0b00100110;// 8bits, transmisión habilitada, asíncrono, alta velocidad
     RCSTA=0b10010000;//habilitado el USART PIC, recepción 8 bits,  habilitada, asíncrono
     SPBRG=22; //Valor aprox.(22,3) para una velocidad de 9600 baudios con un oscilador de 3.579545 Mhz 
}

void bailenLeds() {
    unsigned short int i;
    for (i = 0; i < 10; i++) {
        RA3 = 1;
        __delay_ms(200);
        RA4 = 1;
        __delay_ms(200);
        RA3 = 0;
        __delay_ms(200);
        RA4 = 0;
    }
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

short int EEPROM_search() { 
    
    //Defino variables para la función
    short int esta = 0;
    short int direccion = 0;
    short int numProd = 0;
    short int precio = -1;
    //Busco si lo leido en el puerto serial coincide con algun articulo de EEPROM y en ese caso devuelvo precio
    while (esta < LARGO_ART && direccion < (LARGO_ART + LARGO_PRECIO)*CANT_ART) {
        
        for(int i = 0; i < LARGO_ART; i++) {
            
            //Verifico cada byte en cada artículo
            if(codigoEntrada[i] == eeprom_read(direccion)) {
                esta++;
            }
            else{
                esta = 0;
                i = LARGO_ART; //Para salir del for.
            }
            direccion++;
        }

        direccion = direccion + 3; //Evito el precio de cada artículo
        numProd++;
    }
    
    numProd--; //resto -1 para tener el offset necesario en memoria
    //Me fijo si el producto esta ingresado y el codigo es correcto (esta)    
    if ( (esta == LARGO_ART) && !( prodIngresados & (int) pow(2,numProd) ) ) {   
        
        //Convierto cada cifra del precio para obtenerlo correctamente
        precio =(short int) ( 100*(eeprom_read( numProd*(LARGO_ART + LARGO_PRECIO) + LARGO_ART ) - '0') );
        precio += (short int) ( 10*(eeprom_read( numProd*(LARGO_ART + LARGO_PRECIO) + LARGO_ART + 1) - '0') );
        precio += (short int) ( eeprom_read( numProd*(LARGO_ART + LARGO_PRECIO) + LARGO_ART + 2) - '0');
        
        //Realizo un or para guardar el producto ingresado
        prodIngresados = prodIngresados | ((int) pow(2,numProd));
         
    }
    return precio;
}

void accionesPuertoSerial(){
    short int Aux = 0;
    
    //Realizo suma para checksum
    for (int i = 0; i < LARGO_ART; i++ ) {
        Aux += (codigoEntrada[i] - '0');
    }
    //Verifico checksum
    if (Aux == (codigoEntrada[8] - '0')) {         
        //Busco precio en eeprom, sumo y muestro nueva cuenta (utilizo Aux para utilizar la menor memoria posible)
        Aux = EEPROM_search(); //Guardo precio del articulo ingresado
        
        if ((cuenta + Aux) <= 999) { //Si la cuenta no sobrepasa 99,9, la compra es correcta.
            
            cuenta += Aux;        
            mostrarDigitos(cuenta);            
        }
        else { //Cuenta >99,9: Enciendo led rojo
            RA4 = 1;
            __delay_ms(1000);
            RA4 = 0;            
        }
    }
    else {
        //Checksum erroneo: Enciendo led rojo
        RA4 = 1;
        __delay_ms(1000);
        RA4 = 0;
    }
}

void main(void) {
    
    //Seteo de entradas y salidas
    TRISA = 0x06; //Defino PORTA con RA1(Boton Aceptar) Y RA2(Boton deshacer) como entradas (botones) y el resto salidas (leds(2))
    TRISB = 0x00; //Defino PORTC como salidas (Unidades)
    TRISD = 0x00; //Defino PORTD como salidas (Decenas)
    INTCON = 0b11000000;    //Habilito las interupciones
    PIE1bits.RCIE=1;        //habilita interrupción por recepción.    
    iniciar_usart();        //inicia el módulo USART PIC (uso de puerto serial)
    
    //Cargo los artículos en EEPROM si es que no tiene mi código (supositoriamente)
    if (eeprom_read(0x00) != '0') {
        
        for(short int i = 0; i < CANT_ART; i++) {         
            for(short int j = 0; j < (LARGO_ART + LARGO_PRECIO); j++) { //Cargo char a char!
                eeprom_write(0x00+(LARGO_ART + LARGO_PRECIO)*i + j,(productos[i])[j]);  
            }
        } 
        
    }    
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
    
    short int i = 0;
    short int recibir = 1;
    huboInt = 1;
    
    while(recibir) {
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