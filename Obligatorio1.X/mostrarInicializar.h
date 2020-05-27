/* 
 * File:   mostrarInicializar.h
 * Authors: Julian Ferreira y Marcelo Hernandez
 *
 * Created on 26 de mayo de 2020, 12:24 PM
 */

#ifndef MOSTRARINICIALIZAR_H
#define	MOSTRARINICIALIZAR_H

#include <xc.h>
#include "variablesGlobales.h"

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

void mostrarDigitos(unsigned int num);

void iniciar_usart(void);

void bailenLeds(void);

#endif	/* MOSTRARINICIALIZAR_H */

