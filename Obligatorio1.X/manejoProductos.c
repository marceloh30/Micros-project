#include "manejoProductos.h"

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
            prodIngresados[i] = prodIngresados[i] | pow(2,tp);
            i = 0;// lo pongo a 0 para salir (encontre su byte)

        }
    }
}

char verificarProd(short int tp) {
    char ret = 0;
    for(short int i = 12; i>=0; i--) { //Busco el byte que corresponde para setear bandera de ese TP en 1
        if( tp >= 8*i) {

            tp = tp - 8*i;
            if(prodIngresados[i] & pow(2,tp)) {
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
            prodIngresados[i] = prodIngresados[i] ^ pow(2,tp);
            i = 0;// lo pongo a 0 para salir (encontre su byte)

        }
    }

}


void agregarModificarPrecio(){
    unsigned char tp = 10*(codigoEntrada[1]-'0') + (codigoEntrada[2] - '0');
    if (tp > 0){
        char lower_8bits;
        char upper_8bits;
        char mensaje[10];
        tp--;
        tp = tp * LARGO_PRECIO;
        short int precio;

        //agrego o modifico precio de producto
        precio = 100 * (codigoEntrada[4] - '0')+ 10 * (codigoEntrada[5] - '0') + (codigoEntrada[6]  - '0');
        lower_8bits = precio & 0xff;
        upper_8bits = (precio >> 8) & 0xff;
        eeprom_write(tp ,upper_8bits);
        eeprom_write(tp + 1,lower_8bits);
        sprintf(mensaje, "\nP%d=%d\n", tp/LARGO_PRECIO + 1, precio);
        envioTX(mensaje);
    }
    else{
        envioTX("No hay producto 00");
    }
}
