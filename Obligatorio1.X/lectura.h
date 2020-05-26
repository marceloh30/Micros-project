/* 
 * File:   lectura.h
 * Author: julia
 *
 * Created on 26 de mayo de 2020, 12:20 PM
 */

#ifndef LECTURA_H
#define	LECTURA_H

short int EEPROM_search(unsigned char tp);

void lecturaEtiqueta(void);

char verificacionEntrada(void);

void lecturaMas(void);

void lecturaMenos(void);

void lecturaComando(void);

#endif	/* LECTURA_H */

