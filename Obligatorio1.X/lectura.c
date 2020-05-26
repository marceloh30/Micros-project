#include <xc.h>
#include "manejarProductos.h"
#include "mostrarInicializar.h"
#include "variablesGlobales.h"


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
    for (int i = 0; i < 8; i++ ) {
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
