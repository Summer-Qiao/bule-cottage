/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Template/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    13-April-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "stm32f4xx.h"
#include  <ucos_ii.h>
#include "GET_SET.h"
/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
#include "math.h"
#include "adc.h"
#include "usart.h"
#include "can.h"
#include "robs.h"
#include "walk.h"
#include "String.h"
#include "stm32f4xx_dma.h"
#include "gpio.h"
#include "stm32f4xx_exti.h"
#include "elmo.h"
#include "action_math.h"
#include "collectball.h"

/************************************************************/
/****************驱动器CAN1接口模块****start******************/
int receiveCount;    //临时使用，测试can通信
union Speed
{
	int8_t sdiv[2];
	int16_t stotal;	
}sudu6,sudu8;
int16_t speed6;int16_t speed8;

uint8_t data[4];
uint8_t data1[4];
uint8_t datal[3];
uint16_t encoder_right;
int16_t vell_right;
int16_t vell_right1;
uint8_t left[3];
uint8_t middle[3];
uint8_t right[3];
int16_t colorgroundright;
int16_t colorgroundleft;
u8 whiteball,blackball;


static int speed6_s=0;
static int speed8_s=0;
void sendspeed6(int speed6)
{
	speed6_s=speed6;
}
int getspeed6(void)
{
	return speed6_s;
}
void sendspeed8(int speed8)
{
	speed8_s=speed8;
}
int getspeed8(void)
{
	return speed8_s;
}



void CAN1_RX0_IRQHandler(void)
{
	
	OS_CPU_SR  cpu_sr;
	static uint8_t buffer[8];
	static uint32_t StdId=0;
	OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	CAN_RxMsg(CAN1, &StdId,buffer,8);
	if(StdId == 0x01)
	{
		left[0]=buffer[0];
		left[1]=buffer[1];
		left[2]=buffer[2];
		if(left[0]>=4)
		left[0]=0;
		if(left[1]>=4)
		left[1]=0;
		if(left[2]>=4)
		left[2]=0;
			
	}
		if(StdId == 0x02)
	{
		middle[0]=buffer[0];
		middle[1]=buffer[1];
		middle[2]=buffer[2];
		if(middle[0]>=4)
		middle[0]=0;
		if(middle[1]>=4)
		middle[1]=0;
		if(middle[2]>=4)
		middle[2]=0;
	}
		if(StdId == 0x03)
	{
		right[0]=buffer[0];
		right[1]=buffer[1];
		right[2]=buffer[2];
		if(right[0]>=4)
		right[0]=0;
		if(right[1]>=4)
		right[1]=0;
		if(right[2]>=4)
		right[2]=0;
	}
	if(StdId == 0x68)
	{
	whiteball = buffer[1];
	blackball = buffer[2];
	}
	
	if(StdId == 0x32)//小电下面
	{
		colorgroundright=buffer[0];	
	}
	if(StdId == 0x31)		
	{
		colorgroundleft=buffer[0];	
	}
     if(StdId == 0x16)
	{
		sudu6.sdiv[0]=buffer[2];
		sudu6.sdiv[1]=buffer[3];
		speed6=sudu6.stotal;
		sendspeed6(speed6);
		}

	if(StdId == 0x18)
	{
		sudu8.sdiv[0]=buffer[2];
		sudu8.sdiv[1]=buffer[3];
		speed8=sudu8.stotal;
		sendspeed8(speed8);
	}


	CAN_ClearFlag(CAN1,CAN_FLAG_EWG);
	CAN_ClearFlag(CAN1,CAN_FLAG_EPV);
	CAN_ClearFlag(CAN1,CAN_FLAG_BOF);
	CAN_ClearFlag(CAN1,CAN_FLAG_LEC);
	
	CAN_ClearFlag(CAN1,CAN_FLAG_FMP0);
	CAN_ClearFlag(CAN1,CAN_FLAG_FF0);
	CAN_ClearFlag(CAN1,CAN_FLAG_FOV0);
	CAN_ClearFlag(CAN1,CAN_FLAG_FMP1);
	CAN_ClearFlag(CAN1,CAN_FLAG_FF1);
	CAN_ClearFlag(CAN1,CAN_FLAG_FOV1);
	OSIntExit();
}




/****************驱动器CAN1接口模块****end******************/
/************************************************************/

/*************定时器2******start************/
//每1ms调用一次  用于读取编码器的值和计算坐标
int TIME_s = 0;
extern  OS_EVENT 		*PeriodSem;
//int posx,posy;
int XP_JISHU=0;
void TIM2_IRQHandler(void)
{
	OS_CPU_SR  cpu_sr;
//	static uint8_t countss=0;
//	static uint8_t Count=0;
//	float pos_x;
//  float pos_y;
	OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	if(TIM_GetITStatus(TIM2, TIM_IT_Update)==SET)
	{

		OSSemPost(PeriodSem);
		TIME_s++;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
   }
	 OSIntExit();
}


//定时器1  左编码器中断
void TIM1_UP_TIM10_IRQHandler(void)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	if(TIM_GetITStatus(TIM1, TIM_IT_Update)==SET)    
	{                                                
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
	OSIntExit();
}

//定时器8  右编码器中断
void TIM8_UP_TIM13_IRQHandler(void)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	if(TIM_GetITStatus(TIM8, TIM_IT_Update)==SET)    
	{                                                
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
	}
	OSIntExit();
}

/********************************************************/
/*****************普通定时TIM5*****Start*****************/
void TIM5_IRQHandler(void)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	if(TIM_GetITStatus(TIM5, TIM_IT_Update)==SET)    
	{              
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	}
	OSIntExit();
}

void TIM3_IRQHandler(void)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	if(TIM_GetITStatus(TIM3, TIM_IT_Update)==SET)    
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	
	}
	OSIntExit();
}



//定时器4  
void TIM4_IRQHandler(void)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	if(TIM_GetITStatus(TIM4, TIM_IT_Update)==SET)
	{                                  
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
	OSIntExit();
}



/******************蓝牙串口****************/
void UART4_IRQHandler(void)
{
  

	  
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	
	 if(USART_GetITStatus(UART4, USART_IT_RXNE)==SET)   
	 {
			USART_ClearITPendingBit( UART4,USART_IT_RXNE);
	 }
	 
   OSIntExit();
}

//调试串口中断
void UART5_IRQHandler(void)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	 if(USART_GetITStatus(UART5, USART_IT_RXNE)==SET)   
	 {
		USART_ClearITPendingBit( UART5,USART_IT_RXNE);

	 }
	  OSIntExit();	 
	 
}


/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
   while (1)
   {
   }
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{

  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{

  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
 
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}
/*
static float lasta_1,lasta_2,lastb_1,lastb_2;
void setlasta1(float posx)
{
	lasta_1=posx;
}

 float getlasta1(void)
 {
	 return lasta_1;
 }
void setlasta2(float posx)
{
	lasta_2=posx;
}

 float getlasta2(void)
 {
	 return lasta_2;
 }
 void setlastb1(float posx)
{
	lastb_1=posx;
}

 float getlastb1(void)
 {
	 return lastb_1;
 }
 void setlastb2(float posx)
{
	lastb_2=posx;
}

 float getlastb2(void)
 {
	 return lastb_2;
 } 
*/