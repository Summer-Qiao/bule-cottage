/**
  ******************************************************************************
  * @file     
  * @author  tzy
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  *
  * 
  ******************************************************************************
  */ 
/* Includes -------------------------------------------------------------------*/
#include "action_math.h"
#include "math.h"
#include "GET_SET.h"
/* Private  typedef -----------------------------------------------------------*/
/* Private  define ------------------------------------------------------------*/
/* Private  macro -------------------------------------------------------------*/
/* Private  variables ---------------------------------------------------------*/
//static float Pos_x;
//static float Pos_y;
//static float Angle;
/* Extern   variables ---------------------------------------------------------*/
/* Extern   function prototypes -----------------------------------------------*/
/* Private  function prototypes -----------------------------------------------*/
/* Private  functions ---------------------------------------------------------*/
/* Exported function prototypes -----------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/
/**
  * @brief  
  * @param 
  * @param 
  * @retval 
  */
//���Ǻ���ת��
double Sin(double Angle)
{
	return sin(Angle/180*PI);
}
double Cos(double Angle)
{
	return cos(Angle/180*PI);
}
double Tan(double Angle)
{
	return tan(Angle/180*PI);
}

//��ȫ�����Ǻ���,���ؽǶ�ֵ�����ǻ���ֵ��
double Acos(double Cos)
{
	if(Cos>1)Cos=1;
	if(Cos<-1)Cos=-1;
	return ((180/PI)*acos(Cos));
}

//ʵ���ٶ�ת�����ٶ� 
int VelTransform(float vel)
{
	return ((vel*2000*Reduce_Rate)/(PI*2*R_Wheel));//2000�ߣ�2000��תһȦ�������ٱȣ�ֱ��
}

//���Բ��Բ��
float get_origin_x(int Circle_Radius,int angle_init)
{
	return (Get_POS_Xtemp()-(Circle_Radius*Cos(angle_init)));
}
float get_origin_y(int Circle_Radius,int angle_init)
{
	return (Get_POS_Ytemp()+(Circle_Radius*Sin(angle_init)));
}

float get_cos(int Circle_Radius,int angle_init)
{
	return ((get_origin_y(Circle_Radius,angle_init)-Get_POS_Y())/Circle_Radius);
}
float get_sin(int Circle_Radius,int angle_init)
{
	return ((Get_POS_X()-get_origin_x(Circle_Radius,angle_init))/Circle_Radius);
}

/************************ (C) COPYRIGHT 2015 ACTION *****END OF FILE****/
