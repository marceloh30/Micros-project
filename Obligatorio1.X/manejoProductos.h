#ifndef MANEJARPRODUCTOS_H
#define	MANEJARPRODUCTOS_H

//Librerias
#include <xc.h>
#include <stdio.h>
#include "mostrarInicializar.h"
#include "variablesGlobales.h"

//Firmas
unsigned int pow(unsigned int numero,unsigned int potencia);

void ingresoProd(short int tp);

char verificarProd(short int tp);

void eliminarProd(short int tp);

void agregarModificarPrecio(void);

#endif	/* MANEJARPRODUCTOS_H */

