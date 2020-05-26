#include <xc.h>
#include "lectura.h"
#include "manejarProductos.h"
#include "mostrarInicializar.h"
#include "variablesGlobales.h"



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
