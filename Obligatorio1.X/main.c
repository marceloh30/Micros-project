/*
 * File:   main.c
 * Authors: Julián Ferreira y Marcelo Hernandez
 *
 * Created on 15 de abril de 2020, 06:59 PM
 */
#include "main.h"

void main(void) {
    

    //Seteo de entradas y salidas
    TRISA = 0x01; //una entrada analogica RA0-AN0(Voltaje) y el resto salidas digitales
    TRISE = 0x07; //Defino PORTE con RE1(Aceptar),RE2(deshacer) y RE0(lote) como entradas (botones
    TRISB = 0x00; //Defino PORTC como salidas (Unidades)
    TRISD = 0x00; //Defino PORTD como salidas (Decenas)
    
    //Configuraciones iniciales
    ADCON0 = 0b10000001; //Config para obtener lecturas de A0 a f=Fosc/8 y modulo A/D encendido
    ADCON1 = 0b10001110; //Config para una salida/entrada analogica y las demas digitales    
    INTCON = 0b11000000; //Habilito las interupciones: serial y A/D
    
    //Inicializacion modulo USART PIC
    TRISC = 0b10000000; //pin RX como una entrada digital pin TX como una salida digital
    TXSTA = 0b00100110; //8bits, transmision habilitada, asincrono, alta velocidad
    RCSTA = 0b10010000; //habilitado el USART PIC, recepcion 8 bits,  habilitada, asincrono
    SPBRG = 25;         //Valor  para una velocidad de 9600 baudios con un oscilador de 4 Mhz     
    
    //Otras Inicializaciones
    ADIF = 0;
    ADIE = 1;            //Habilito int. A/D
    RCIE = 1;            //habilita interrupcion por recepcion.     
    cuenta = 0;
    auxCuenta = 0;
    mostrarDigitos(cuenta);
        
    //Bucle principal del programa
   
    while(1) {
        
        if(RE0) {       //Boton aceptar
            while(RE0);
            accionesAceptar();
        }
        else if(RE1) {  //Boton deshacer
            while(RE1);
            accionesDeshacer();
        }
        else if(RE2) {  //Boton lote
            while(RE2);
            if (cuenta == 0){
                cierreLotePedido = 1;
                char strLote[32];
                sprintf(strLote,"\nCierre, L:%d,N:%d,T:%d\n", nroLote, ventasLote, montosLote);
                envioTX(strLote);
                for(char i = 0; i<10; i++){
                    __delay_ms(100);//Preguntar a diego asegurar que no apague las interupciones
                }
            }
        }                
        else if(huboInt) {
            huboInt = 0; 
            accionesPuertoSerial();
            for(char i = 0; i < SERIALMAX; i++){
                codigoEntrada[i] = 0;
            }
        }
        else if (adresult > 0) {

            adresult = adresult*10*5/1023; //convierto resultado A/D en voltios (5V equivale a 1024)
            char bufferMsj[16];
            sprintf(bufferMsj,"V=%d.%dV\n", adresult/10, adresult%10); //conversionVoltaje reinicia adresult y devuelve valor en Voltios
            envioTX(bufferMsj); 
            adresult = 0;
        }
        
        
    }
    
}

//rutina de atencion a la interrupcion por Rx serial
void __interrupt() int_usart() {
    
    if(RCSTAbits.FERR){
        char basura = RCREG;
    }
    if (RCSTAbits.OERR){ // check for overrun.
        RCSTAbits.CREN=0; // clear receiver
        RCSTAbits.CREN=1; // re-enable
    }
    if(RCIF) {
        if((codigoEntrada[serial] = RCREG) != 0x0D && (codigoEntrada[serial]) != 0x0A && serial < (SERIALMAX-1)) { //Verifico que no me sobrepase de los datos esperados! 
            serial++;
        }
        else{
            serial = 0;
            huboInt = 1;
        }
    }
    if(ADIF) {       
        ADIF = 0; //Limpio flag
        adresult = (ADRESH<<8)+ADRESL;
    }
    
}
