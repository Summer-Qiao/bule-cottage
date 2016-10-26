
/**
  ******************************************************************************
  * @file    
  * @author  Tzy
  * @version 
  * @date   
  * @brief   This file contains the headers of action_math.c
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
#ifndef __ACTION_MATH_H__
#define __ACTION_MATH_H__

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
double Sin(double Angle);
double Cos(double Angle);
double Tan(double Angle);
double Acos(double Cos);
int VelTransform(float vel);
float get_origin_x(int Circle_Radius,int angle_init);
float get_origin_y(int Circle_Radius,int angle_init);
float get_cos(int Circle_Radius,int angle_init);
float get_sin(int Circle_Radius,int angle_init);

#endif

/******************* (C) COPYRIGHT 2015 ACTION *****END OF FILE****/

