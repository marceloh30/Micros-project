/* 
 * File:   mostrarInicializar.h
 * Authors: Julian Ferreira y Marcelo Hernandez
 *
 * Created on 26 de mayo de 2020, 12:24 PM
 */

#ifndef MOSTRARINICIALIZAR_H
#define	MOSTRARINICIALIZAR_H

//Librerias
#include <xc.h>
#include <string.h>
#include "variablesGlobales.h"



//Firmas
void mostrarDigitos(unsigned int num);

void bailenLeds(void);

void envioTX(char *mensaje);

#endif	/* MOSTRARINICIALIZAR_H */

