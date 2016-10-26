#include "stm32f4xx_can.h"

#define CAN_ENABLE 1

void CAN_Config(CAN_TypeDef* CANx, 
				uint32_t CAN_BaudRate,
				GPIO_TypeDef * GPIOx,
				uint16_t CAN_RxPin,
				uint16_t CAN_TxPin);

uint8_t CAN_RxMsg(CAN_TypeDef* CANx,
				  uint32_t * StdId,
				  uint8_t * buf,
				  uint8_t len);

uint8_t CAN_TxMsg(CAN_TypeDef* CANx,
				  uint32_t StdId,
				  uint8_t * buf,
				  uint8_t len);


void setrollvel_L(int16_t val);
int16_t getRollVel_L(void);
void setrollvel_R(int16_t val);
int16_t getRollVel_R(void);

void SetColor_l(char val);
void SetColor_r(char val);

char getcollor_l(void);
char getcollor_r(void);
