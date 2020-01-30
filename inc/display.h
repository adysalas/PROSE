/*
 * display.h
 *
 *  Created on: Dec 17, 2019
 *      Author: Saldory
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

extern char buff[100];


void sendString(char* string);
void displayError(void);
void displayNADA(void);
void displayString(int value,int cluster);
void displayString2(int value);
void displei(int k);
void displayErrorID(int error);
void mostra_tudo(void);
void display_luxs(void);
void valid(int a, int c);


#endif /* DISPLAY_H_ */
