/**
  ******************************************************************************
  * @file     
  * @author  lxy
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
#include "GET_SET.h"
#include "elmo.h"
/* Private  typedef -----------------------------------------------------------*/
/* Private  define ------------------------------------------------------------*/
/* Private  macro -------------------------------------------------------------*/
/* Private  variables ---------------------------------------------------------*/
static float Pos_x;
static float Pos_y;
static float offset_x;
static float offset_y;
static float Pos_xtemp;                      //temporary value of coordinate in X direction 
static float Pos_ytemp;                      //temporary value of coordinate in Y direction
static float LaserValue_TEMP;
static union can_message receive_current[4];
static int   Pos[4];
static int   Vel[4];
static float Angle;
static int Camera_Height;
static float windSpeed=0;
static float adjVel=0;
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
	void Set_POS_X(float val)
	{
		Pos_x=val;
	}
	void Set_POS_Y(float val)
	{
		Pos_y=val;
	}
	void Set_Offset_X(float val)
	{
		offset_x=val;
	}
	void Set_Offset_Y(float val)
	{
		offset_y=val;
	}
	void Set_POS_Xtemp(float val)
	{
		Pos_xtemp=val;
	}
	void Set_POS_Ytemp(float val)
	{
		Pos_ytemp=val;
	}
	
	void TempUpdate(void)
	{
		Set_POS_Xtemp(Get_POS_X());
		Set_POS_Ytemp(Get_POS_X());
	}
	
	void Set_Angle(float val)
	{
		Angle=val;
	}
	void Set_Current(float val,int num)
	{
		receive_current[num].dataf=val;	
	}
	void Set_Pos(int val,int num)
	{
		Pos[num]=val;
	}
	void Set_Vel(int vel,int num)
	{
		Vel[num]=vel;
	}
	void Set_Camera_Height(int16_t val)
	{
		Camera_Height=val;
	}

	void Set_Windspeed(float vel)
	{
      windSpeed=vel;
	}
	void Set_AdjVel(int vel)
	{
      adjVel=vel;
	}
	
	void Set_LaserValue_TEMP(float val)
	{
		  LaserValue_TEMP=val; 
	}
	
	
	
	
	
	
	
	
	
	
	
	float Get_POS_X(void)
	{
	  return (Pos_x-offset_x);
	}
	float Get_POS_Y(void)
	{
		return (Pos_y-offset_y);
	}
	float Get_Angle(void)
	{
		return Angle;
	}
	float Get_POS_Xtemp(void)
	{
		return Pos_xtemp;
	}
	float Get_POS_Ytemp(void)
	{
		return Pos_ytemp;
	}
	float Get_Current(int num)
	{
		return (receive_current[num].dataf);
	}
	int Get_Pos(int num)
	{
		return Pos[num];
	}
	
	int Get_Vel(int num)
	{
		return Vel[num];
	}
	int16_t Get_Camera_Height(void)
	{
		return Camera_Height;
	}

	
	float Get_Windspeed(void)
	{
		return windSpeed;
	}
	
	int Get_AdjVel(void)
	{
		 return adjVel;
	}
	float Get_LaserValue_TEMP()
	{
		return LaserValue_TEMP;
	}
		
/************************ (C) COPYRIGHT 2015 ACTION *****END OF FILE****/
