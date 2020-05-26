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

//Defino variables estaticas (debido a que cruzan funciones)
static unsigned short int cuenta, auxCuenta;
static short int huboInt = 0;
static char serial = 0;
static char modoDebug = 0;
static short int productoIngresado;
static short int numProd;
static char codigoEntrada[10];
static char ventasLote = 0;
static unsigned short int montosLote = 0;
static char nroLote = 1;
unsigned char prodIngresados[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};; //99 lugares para 99 productos

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

unsigned int pow(unsigned int numero,unsigned int potencia){
    unsigned int resultado = 0;
    
    for (int i = 0; i <= potencia; i++){
        if (i == 0){
        resultado = 1;
        }
        else{
            resultado = resultado * numero;
        }
    }
    return resultado;
}

void ingresoProd(short int tp) {

    for(short int i = 12; i>=0; i--) { //Busco el byte que corresponde para setear bandera de ese TP en 1
        if( tp >= 8*i) {

            tp = tp - 8*i;
            prodIngresados[i] = prodIngresados[i] | ((unsigned int)pow(2,tp));
            i = 0;// lo pongo a 0 para salir (encontre su byte)

        }
    }
}

char verificarProd(short int tp) {
    char ret = 0;
    for(short int i = 12; i>=0; i--) { //Busco el byte que corresponde para setear bandera de ese TP en 1
        if( tp >= 8*i) {

            tp = tp - 8*i;
            if(prodIngresados[i] & ((unsigned int)pow(2,tp))) {
                ret = 1;
            }
            i = 0;// lo pongo a 0 para salir (encontre su byte)

        }
    }

    return ret;
}

void eliminarProd(short int tp){
        for(short int i = 12; i>=0; i--) { //Busco el byte que corresponde para setear bandera de ese TP en 1
        if( tp >= 8*i) {

            tp = tp - 8*i;
            prodIngresados[i] = prodIngresados[i] ^ ((unsigned int)pow(2,tp));
            i = 0;// lo pongo a 0 para salir (encontre su byte)

        }
    }

}
 
//Agrego con un or y verifico con un and!


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

void accionesAceptar(){
    //Vuelvo todo a su estado "Original"
    ventasLote++;
    montosLote+=cuenta;
    cuenta = 0;
    auxCuenta = 0;
    for(short int i = 0; i < 13; i++){
        prodIngresados[i] = 0;
    }
    mostrarDigitos(cuenta);
    bailenLeds();
}

void accionesDeshacer(){
    if (cuenta != auxCuenta){
        cuenta = auxCuenta;
        //Elimino el ultimo producto ingresado correctamente de la lista de productos ingresados
        eliminarProd(productoIngresado); 
        mostrarDigitos(cuenta);
    }
}

short int EEPROM_search(unsigned char tp) { 
    
    short int precio;
    tp--;
    tp = tp*2;
    precio = (eeprom_read(tp) << 8) | (eeprom_read(tp+1)); //FALTA AGREGAR Comprobante de tp=>0
    
    if( (precio < 0 || precio > 999) || verificarProd(tp/2)){
        precio = -1;
    }
    
    
    return precio;
}

void lecturaEtiqueta(){
    short int Aux = 0;
    
    //Realizo suma para checksum
    for (int i = 0; i < LARGO_ART; i++ ) {
        Aux += (codigoEntrada[i] - '0');
    }
    //Verifico checksum
    if ( (Aux%10) == (codigoEntrada[8] - '0')) {         
        //Busco precio en eeprom, sumo y muestro nueva cuenta (utilizo Aux para utilizar la menor memoria posible)
        unsigned char tp = 10*(codigoEntrada[0]-'0') + (codigoEntrada[1] - '0'); //tomo el valor de tipo de producto
        Aux = EEPROM_search(tp); //Guardo precio del articulo ingresado
        
        if ((cuenta + Aux) <= 999 && Aux != -1) { //Si la cuenta no sobrepasa 99,9, la compra es correcta.
            tp--;
            ingresoProd(tp);
            productoIngresado = tp; //Guardo el ultimo producto ingresado correctametne 
            auxCuenta = cuenta;
            cuenta += Aux;        
            mostrarDigitos(cuenta);     
            RA3 = 1; //Enciendo Led verde
            __delay_ms(1000);
            RA3 = 0; 
     
        }
        else { //Cuenta >99,9: Enciendo led rojo
        
            RA5 = 1;
            __delay_ms(1000);
            RA5 = 0;    
            
        }
    }
    else {
        //Checksum erroneo: Enciendo led rojo
        RA5 = 1;
        __delay_ms(1000);
        RA5 = 0;
        
    }
}

int verificacionEntrada(){ // Verifico para el ingreso de consulta que desea modificar o agregar un producto
    int i = 1;
    int ret = 0;
    while(((codigoEntrada[i] <= '9') && (codigoEntrada[i] >= '0')) || codigoEntrada[i] == '='){
        i++;
        ret++;
    }
    return ret;
}

void lecturaMas(){
    if (codigoEntrada[1] == 'L'){
        //cierreLote()
    }
    else if(verificacionEntrada() == 5){
        //agregarModificarPrecio()
    }
    else if(codigoEntrada[1] == 'D'){
        modoDebug = 1;
    }
    else{
        //codigoNoreconocido();
    }
}

void lecturaMenos(){
    if(codigoEntrada[1] == 'D'){
        modoDebug = 0;
        //Se podria enviar un mensaje que diga modo debug desactivado o algo del estilo 
    }
    else{
        //codigoNoreconocido();
    }
}

void lecturaComando(){
    if(codigoEntrada[0] == '?'){
        //lecturaConsulta();
    }
    else if(codigoEntrada[0] == '+'){
        lecturaMas();
    }
    else{
        lecturaMenos();
    }
}

void accionesPuertoSerial(){
    if ((codigoEntrada[0] == '?') || (codigoEntrada[0] == '+') || (codigoEntrada[0] == '-')){
        lecturaComando();
    }
    else if(codigoEntrada[0] <= '9' && codigoEntrada[0] >= '0'){
        lecturaEtiqueta();
    }
    else{
        //Entrada erronea: Enciendo led rojo
        RA5 = 1;
        __delay_ms(1000);
        RA5 = 0;
    }
        
}

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
