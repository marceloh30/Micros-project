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
#include <string.h>


unsigned short int cuenta, auxCuenta;
static short int huboInt = 0;
char codigoEntrada[9];


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

short int EEPROM_search(){
    short int esta = 0;
    short int direccion = 0;
    short int numProd = 0;
    short int precio;
    while (esta < 8 || direccion < 96){
        for(int i = 0; (i < LARGO_ART) && (esta != 0); i++){
            if(codigoEntrada[i] == eeprom_read(direccion)){
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
        EEPROM_search();
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