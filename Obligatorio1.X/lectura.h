#ifndef LECTURA_H
#define	LECTURA_H

//Librerias
#include <xc.h>
#include "manejarProductos.h"
#include "mostrarInicializar.h"
#include "variablesGlobales.h"

//Firmas
short int EEPROM_search(unsigned char tp);

void lecturaEtiqueta(void);

char verificacionEntrada(void);

void cierreDeLote(void);

void lecturaMas(void);

void lecturaMenos(void);

void lecturaConsulta(void); 

void lecturaComando(void);

#endif	/* LECTURA_H */

