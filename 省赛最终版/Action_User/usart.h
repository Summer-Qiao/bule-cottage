#ifndef __USART_H
#define __USART_H

#include "stdint.h"
#include "stm32f4xx.h"
void setx(float posx);
 void sety(float posy);
void USART1_Init(uint32_t BaudRate);
void UART5_Init(uint32_t BaudRate);
 float gety(void) ;
float getAngle(void);
 float getx(void);
float getXpos(void);
float getYpos(void);
void cleari();
int geti();
void turnxy(void);
void int32tostr(int32_t num, uint8_t str[]);
void UART5_SendString(uint8_t Str[]);
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);
void SetX(float m);
void setAngle2(float angle);
void SetY(float m);
#endif

