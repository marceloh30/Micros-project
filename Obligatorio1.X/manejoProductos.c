#include <xc.h>
#include "manejarProductos.h"
#include "mostrarInicializar.h"
#include "variablesGlobales.h"


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

    for(short char i = 12; i>=0; i--) { //Busco el byte que corresponde para setear bandera de ese TP en 1
        if( tp >= 8*i) {

            tp = tp - 8*i;
            prodIngresados[i] = prodIngresados[i] | pow(2,tp);
            i = 0;// lo pongo a 0 para salir (encontre su byte)

        }
    }
}

char verificarProd(short int tp) {
    char ret = 0;
    for(short char i = 12; i>=0; i--) { //Busco el byte que corresponde para setear bandera de ese TP en 1
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
        for(short char i = 12; i>=0; i--) { //Busco el byte que corresponde para setear bandera de ese TP en 1
        if( tp >= 8*i) {

            tp = tp - 8*i;
            prodIngresados[i] = prodIngresados[i] ^ pow(2,tp);
            i = 0;// lo pongo a 0 para salir (encontre su byte)

        }
    }

}
