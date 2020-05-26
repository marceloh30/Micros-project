/* 
 * File:   variablesGlobales.h
 * Author: julia
 *
 * Created on 26 de mayo de 2020, 03:38 PM
 */

#ifndef VARIABLESGLOBALES_H
#define	VARIABLESGLOBALES_H
#define _XTAL_FREQ 4000000 //Frec. de Cristal que usamos (cristal de un NTSC color television receivers)

//Defino variables estaticas (debido a que cruzan funciones)
extern unsigned short int cuenta, auxCuenta;
extern short int huboInt;
extern char serial;
extern char modoDebug;
extern short int productoIngresado;
extern short int numProd;
extern char codigoEntrada[10];
extern char ventasLote;
extern unsigned short int montosLote;
extern char nroLote;
extern unsigned char prodIngresados[13]; //99 lugares para 99 productos

#endif	/* VARIABLESGLOBALES_H */

