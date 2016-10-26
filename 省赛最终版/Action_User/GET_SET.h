/**
  ******************************************************************************
  * @file    
  * @author  Lxy Action
  * @version 
  * @date   
  * @brief   This file contains the headers of 
  ******************************************************************************
  * @attention
  *
  *
  * 
  * 
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GET_SET_H
#define __GET_SET_H

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define PI 3.1415926
#define Reduce_Rate 624/35
#define R_Wheel 50.8

#define OUT_OF_SIGHT   0

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Set_POS_X(float val);
void Set_POS_Y(float val);
void Set_Offset_X(float val);
void Set_Offset_Y(float val);
void Set_POS_Xtemp(float val);
void Set_POS_Ytemp(float val);
void Set_LaserValue_TEMP(float val);
void TempUpdate(void);


void Set_Angle(float val);
void Set_Current(float val,int num);
void Set_Pos(int val,int num);
void Set_Vel(int vel,int num);
void Set_Windspeed(float vel);
void Set_AdjVel(int vel);	
void Set_Camera_Height(int16_t val);
int16_t Get_Camera_Height(void);

float Get_POS_X(void);
float Get_POS_Y(void);
float Get_POS_Xtemp(void);
float Get_POS_Ytemp(void);
float Get_Angle(void);
float Get_Current(int num);
int 	Get_Pos(int num);
int   Get_Vel(int num);
float Get_Windspeed(void);
int   Get_AdjVel(void);
float Get_LaserValue_TEMP(void);
#endif

/******************* (C) COPYRIGHT 2015 ACTION *****END OF FILE****/

