/*
 * File:   main.c
 * Authors: Julián Ferreira y Marcelo Hernandez
 *
 * Created on 15 de abril de 2020, 06:59 PM
 */

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
