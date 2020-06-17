#include "lectura.h"

short int EEPROM_search(unsigned char tp) { 
    
    short int precio;
    tp--;
    tp = tp*LARGO_PRECIO;
    precio = (eeprom_read(tp) << LARGO_ART) | (eeprom_read(tp+1)); //FALTA AGREGAR Comprobante de tp=>0
    
    if( (precio < 0 || precio > PRECIOMAX) || verificarProd(tp/LARGO_PRECIO)) {
        precio = -1;
    }
    
    return precio;
}

void lecturaEtiqueta() {
    short int Aux = 0;
    unsigned char letra = 0;
    
    //Realizo suma para checksum
    for (int i = 0; i < LARGO_ART; i++ ) {
        if(codigoEntrada[i] >= '0' && codigoEntrada[i] <= '9'){
            Aux += (codigoEntrada[i] - '0');
        }
        else{
            letra = 1;
        }
    }
    //Verifico checksum
    if ( ((Aux%10) == (codigoEntrada[8] - '0')) && letra == 0) {         
        //Busco precio en eeprom, sumo y muestro nueva cuenta (utilizo Aux para utilizar la menor memoria posible)
        unsigned char tp = 10*(codigoEntrada[0]-'0') + (codigoEntrada[1] - '0'); //tomo el valor de tipo de producto
        Aux = EEPROM_search(tp); //Guardo precio del articulo ingresado
        
        if ((cuenta + Aux) <= PRECIOMAX && Aux != -1) { //Si la cuenta no sobrepasa 99,9, la compra es correcta.
            if(modoDebug){
                envioTX("Producto ingresado\n");
            }
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
        else { //Cuenta > PRECIO_MAX o caso erroneo: Enciendo led rojo
        
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

char verificacionEntrada() { //Verifico el ingreso para modificar o agregar precio (luego del '+' espero:"NN=NNN", N=Numero del 0 al 9)
    char i = 1;
    char ret = 0;
    while( (((codigoEntrada[i] <= '9') && (codigoEntrada[i] >= '0')) || (codigoEntrada[i] == '=' && i == 3)) && (i<=6) ) {
        i++;
        ret++;
    }
    return ret;
}

void cierreDeLote() {
    
    if (cierreLotePedido == 0) { //Si no se pidio cierre de lote, envio datos 
        char strLote[32];
        //ver como funciona la cadena para reservar los bytes necesarios: char *strLote=""??
        sprintf(strLote,"\nCierre,L:%d,N:%d,T:%d\n", nroLote, ventasLote, montosLote);
        envioTX(strLote);
    }
    else{
        cierreLotePedido = 0;
    }
    //Aqui, cierro lote en ambos casos:
    nroLote++;
    ventasLote=0;
    montosLote=0;
}

void lecturaMas() {
    if (codigoEntrada[1] == 'L') {
        cierreDeLote();//Fuerza cierre de lote
    }
    else if(verificacionEntrada() == 6){ //Es 6 para verificar por ejemplo 12=203: 6 caracteres!
        agregarModificarPrecio();
    }
    else if(codigoEntrada[1] == 'D') {
        modoDebug = 1;
    }
    else{
        envioTX(strError);// producto no encontrado
    }
}

void lecturaMenos() {
    if(codigoEntrada[1] == 'D') {
        modoDebug = 0;
        //Se podria enviar un mensaje que diga modo debug desactivado o algo del estilo 
    }
    else{
        envioTX(strError);
    }
}

void consultaPrecio(short int articulo) {
    char mensaje[26];
    if(articulo > 0){
        articulo--;
        articulo = articulo * LARGO_PRECIO;
        short int precio = (eeprom_read(articulo) << LARGO_ART) | (eeprom_read(articulo+1));

        if (precio > PRECIOMAX || precio < 0) {
            sprintf(mensaje, "\nProducto no encontrado\n");
            envioTX(mensaje);// producto no encontrado
        }
        else{
            sprintf(mensaje, "TP: %d P: %d,%d Euros", articulo/LARGO_PRECIO + 1, precio/10, precio%10);
            envioTX(mensaje);// producto y precio
        }
    }
    else{
        envioTX("No existe producto 00");
    }
    
}

void lecturaConsulta() { //Recibi '?' en 1er byte: Verifico los siguientes.

    if (codigoEntrada[1] == 0x0D || codigoEntrada[1] == 0x0A) {     //Consulta Estado
        //Lei solo '?': consulta estado
        if (cuenta != 0) {
            envioTX("\nEstado: Activo\n");
        }
        else {
            envioTX("\nEstado: En espera\n"); //falta el barra n que no encontre
        }
    }
    else if(codigoEntrada[1] == 'L') {                              //Consulta Lote
        char strLote[32];
        //ver como funciona la cadena para reservar los bytes necesarios: char *strLote=""??
        sprintf(strLote,"\nL:%d,N:%d,T:%d\n", nroLote, ventasLote, montosLote);
        envioTX(strLote);

    }
    else if(codigoEntrada[1] == 'V') {                            //Consulta Voltaje
        //Inicializo conversion:
        pedidoVoltaje = 1;
    }
    
    else if( codigoEntrada[1] <= '9' && codigoEntrada[1] >= '0' && codigoEntrada[2] <= '9' && codigoEntrada[2] >= '0' ) { //Consulta Precio

        unsigned short int articulo = 10*(codigoEntrada[1] - '0') + (codigoEntrada[2] - '0');

        consultaPrecio(articulo);

    }
    else {
        envioTX(strError);//Consulta erronea
    }                              

 
}

void lecturaComando() {
    if(codigoEntrada[0] == '?') {
        lecturaConsulta();
    }
    else if(codigoEntrada[0] == '+') {
        lecturaMas();
    }
    else{ //Ya verifique que fuese uno de los 3 ('?','+' o '-')
        lecturaMenos();
    }
}
