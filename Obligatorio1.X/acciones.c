#include "acciones.h"
#include "main.h"

void accionesAceptar() {
    //Vuelvo todo a su estado "Original"
    ventasLote++;
    montosLote+=cuenta;
    cuenta = 0;
    auxCuenta = 0;
    for(short int i = 0; i < 13; i++){
        prodIngresados[i] = 0;
    }
    if(modoDebug){
        char strLote[32];
        //ver como funciona la cadena para reservar los bytes necesarios: char *strLote=""??
        sprintf(strLote,"\nL:%d,N:%d,T:%d\n", nroLote, ventasLote, montosLote);
        envioTX(strLote);
    }
    mostrarDigitos(cuenta);
    bailenLeds();
}

void accionesDeshacer() {
    if (cuenta != auxCuenta) {
        cuenta = auxCuenta;
        //Elimino el ultimo producto ingresado correctamente de la lista de productos ingresados
        eliminarProd(productoIngresado); 
        mostrarDigitos(cuenta);
    }
    if (modoDebug){
        envioTX("Producto cancelado");
    }
}

void accionesPuertoSerial() {
    if ((codigoEntrada[0] == '?') || (codigoEntrada[0] == '+') || (codigoEntrada[0] == '-')) {
        lecturaComando();
    }
    else if(codigoEntrada[0] <= '9' && codigoEntrada[0] >= '0') {
        if (modoDebug){
            char mensaje[12];
            sprintf(mensaje,"E:%s", codigoEntrada);
            envioTX(mensaje);
        }
        lecturaEtiqueta();
    }
    else{
        //Entrada erronea: Enciendo led rojo
        RA5 = 1;
        __delay_ms(1000);
        RA5 = 0;
    }
        
}

void escrituraDeCierre(){
    
    char lower_8bits;
    char upper_8bits;
    lower_8bits = montosLote & 0xff;
    upper_8bits = (montosLote >> 8) & 0xff;
    eeprom_write(252, upper_8bits);
    eeprom_write(253, lower_8bits);
    eeprom_write(254, ventasLote);
    eeprom_write(255, nroLote);
    RC0 = 1;
}

