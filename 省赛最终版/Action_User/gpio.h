#ifndef __GPIO_H

#define __GPIO_H

#include "stm32f4xx_gpio.h"


#define LIGHT_UNLASHFAN   GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7)
#define LIGHT_GRABPOST    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)
#define LIGHT_GRABFAN 	 	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)
#define LIGHT_HIGH     		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_10)
#define SWITCH_END_0   	 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_11)
#define SWITCH_END_1   	 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_12)
float stop2(void);
#define BEEP_ON          GPIO_SetBits(GPIOE,GPIO_Pin_2)
#define BEEP_OFF         GPIO_ResetBits(GPIOE,GPIO_Pin_2)
 
void GPIO_Init_Pins(GPIO_TypeDef * GPIOx,
					uint16_t GPIO_Pin,
					GPIOMode_TypeDef GPIO_Mode);
void sendspeed6(int speed6);
void sendspeed8(int speed8);
int getspeed6(void);
int getspeed8(void);

void KeyInit(void);
#define KEY_L				GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)
#define KEY_R 			GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1)


#define PhoElec_LF 	(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12)==0)
#define PhoElec_RF 	(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7)==0)

#define PhoElec_L 	(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1))
#define PhoElec_R 	(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0))


#define OUTSIDE     (real_x>4300||real_y>4300||real_x<500||real_y<500)
#define INSIDE     (real_x>1100&&real_y>1600&&real_x<3700&&real_y<3200)
void BeepInit(void);
	
#endif
