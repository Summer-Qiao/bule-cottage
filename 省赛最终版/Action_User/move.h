#ifndef __MOVE_H
#define __MOVE_H
#include "stm32f4xx_rcc.h"
void angleline(float exangle,float speed);
void velocitydecline( int di,int mark);
float getexa(float exangle);
float getex(void);
float getv(void);
 void setnx(int nx);
void freedom(	int bound12,int bound23,int bound34,int bound14,int num);
void setex(float posx);
int judge_j(uint8_t*left,uint8_t*middle,uint8_t*right);
int max(uint8_t*area);
void findball(int j);
int send_CAN(void);
int judge(float x,float y);
void CloseLeftDoor(void);
void CloseRightDoor(void);
void OpenLeftDoor(void);
void OpenRightDoor(void);
void go_small(float x1,float y1,float x2,float y2,float advance1,float advance2,float V);
void forward_anglemove(int expectangle,u16 speed, u8 proportion);
void back_anglemove(int expectangle,u16 speed, u8 proportion);
#endif


