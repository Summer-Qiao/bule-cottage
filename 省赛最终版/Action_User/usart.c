#include "usart.h"
#include "math.h"
#include "misc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stdarg.h"
#include  <includes.h>
#include "stdio.h"
#include "stm32f4xx_usart.h"

#define LENGTH 3


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */


int fputc(int ch, FILE *f)
{
	 USART_SendData(UART5, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET)
  {}

  return ch;
}



void USART1_Init(uint32_t BaudRate)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART3时钟
 
	//串口3对应引脚复用映射
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_USART1); //GPIOC10复用为USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1); //GPIOC11复用为USART3
	
	//USART3端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOC10与GPIOC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = BaudRate;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1 	
	//USART_ClearFlag(USART3, USART_FLAG_TC);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

}

void UART5_Init(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* USARTx configured as follow:
	- BaudRate = 57600 baud  
	- Word Length = 8 Bits
	- One Stop Bit
	- No parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//------------------------------------------------------------
	 
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource2,  GPIO_AF_UART5);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(UART5, & USART_InitStructure);
  
	//////////   设置UART5中断       ///////////////
	NVIC_InitStructure.NVIC_IRQChannel=UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2);
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
	/* Enable USART */
	USART_Cmd(UART5, ENABLE);
 //------------------------------------------------------------
	//使能USART5接收中断,
 
}
static float angle=0;
static float xpos=0, ypos=0,xpos2=0, ypos2=0;
static float x_err=0,y_err=0;
static float errl=0;


void setAngle2(float angle)
{
	errl+=(angle-getAngle());
	if(errl>180) errl-=360;
	if(errl<-180) errl+=360;
}
float getAngle(void)//陀螺仪角度
{
	
	if(angle>180)
	{
		angle=angle-360;
	}
	return angle+errl;
}


float getXpos(void)//x坐标
{
	return	xpos2;
}
void SetX(float m)
{
	x_err+=(m-getXpos());

}

void SetY(float m)
{
	y_err+=(m-getYpos());
}


float getYpos(void)//y坐标
{
	return ypos2;
}

void turnxy(void)
{
	xpos2=xpos*cos(-errl*0.01745)+ypos*sin(-errl*0.01745)+x_err;
	ypos2=ypos*cos(-errl*0.01745)-xpos*sin(-errl*0.01745)+y_err;
	USART_OUT(UART5,(u8 *)"x1:%d\t x2:%d\t ",(int)xpos,(int)xpos2);
	USART_OUT(UART5,(u8 *)"y1:%d\t y2:%d\t\r\n ",(int)ypos,(int)ypos2);
}

void int32tostr(int32_t num, uint8_t str[])      /*0到65535  str大于等于6位*/
{
    uint8_t str_tem[12];
    uint8_t i = 0, j = 0;
	if (num >=0)
	{
	    do{
	        str_tem[i++]=num%10 + 48;
		    num /= 10;
        }while(num!=0);

        for(j=0; j < i;j++)
        {
            str[j] = str_tem[i - j - 1];
        }
        str[i] = '\0';
	}
	else
    {
        num = -1 * num;
	    do{
	    	str_tem[i++]=num%10 + 48;
		    num /= 10;
        }while(num!=0);

        str[0] = '-';
        for(j=1; j < i+1;j++)
  
        {
            str[j] = str_tem[i - j];
        }
        str[i+1] = '\0';
    }
}

void UART5_SendString(uint8_t Str[])     /*UART5发送字符串,无0X0D, 0x0A*/
{
	while(*Str != 0)
	{
		USART_SendData(UART5, *Str++);
	}
}



/****************陀螺仪串口接受中断****start****************/

static float pos_x=0;
static float pos_y=0;
static float zangle=0;
static float xangle=0;
static float yangle=0;
static float w_z=0;
void USART1_IRQHandler(void)
{	 
	static uint8_t ch;
	static union
  {
	 uint8_t data[24];
	 float ActVal[6];
  }posture;
	static uint8_t count=0;
	static uint8_t i=0;
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();/* Tell uC/OS-II that we are starting an ISR*/
	OSIntNesting++;
	OS_EXIT_CRITICAL();

	if(USART_GetITStatus(USART1, USART_IT_RXNE)==SET)   
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		ch=USART_ReceiveData(USART1);
		 switch(count)
		 {
			 case 0:
				 if(ch==0x0d)
					 count++;
				 else
					 count=0;
				 break;
				 
			 case 1:
				 if(ch==0x0a)
				 {
					 i=0;
					 count++;
				 }
				 else if(ch==0x0d);
				 else
					 count=0;
				 break;
				 
			 case 2:
				 posture.data[i]=ch;
			   i++;
			   if(i>=24)
				 {
					 i=0;
					 count++;
				 }
				 break;
				 
			 case 3:
				 if(ch==0x0a)
					 count++;
				 else
					 count=0;
				 break;
				 
			 case 4:
				 if(ch==0x0d)
				 {
  				 angle=posture.ActVal[0]+90;
	  		   xangle=posture.ActVal[1];
		  	   yangle=posture.ActVal[2];
			     xpos =-posture.ActVal[3]+2351;
			     ypos =-posture.ActVal[4]+50;
			     w_z   =posture.ActVal[5];
					 setx(xpos);
					 sety(ypos);
				 }
			   count=0;
				 break;
			 
			 default:
				 count=0;
			   break;		 
		 }
		 
		 
	 }
	OSIntExit();
}





char *itoa(int value, char *string, int radix);

 /****************************************************************************
* 名    称：void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...)
* 功    能：格式化串口输出函数
* 入口参数：USARTx:  指定串口
			Data：   发送数组
			...:     不定参数
* 出口参数：无
* 说    明：格式化串口输出函数
        	"\r"	回车符	   USART_OUT(USART1, "abcdefg\r")   
			"\n"	换行符	   USART_OUT(USART1, "abcdefg\r\n")
			"%s"	字符串	   USART_OUT(USART1, "字符串是：%s","abcdefg")
			"%d"	十进制	   USART_OUT(USART1, "a=%d",10)
* 调用方法：无 
****************************************************************************/
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...){ 
	const char *s;
    int d;
    char buf[16];
    va_list ap;
    va_start(ap, Data);

	while(*Data!=0){				                          //判断是否到达字符串结束符
		if(*Data==0x5c){									  //'\'
			switch (*++Data){
				case 'r':							          //回车符
					USART_SendData(USARTx, 0x0d);	   

					Data++;
					break;
				case 'n':							          //换行符
					USART_SendData(USARTx, 0x0a);	
					Data++;
					break;
				
				default:
					Data++;
				    break;
			}
			
			 
		}
		else if(*Data=='%'){									  //
			switch (*++Data){				
				case 's':										  //字符串
                	s = va_arg(ap, const char *);
                	for ( ; *s; s++) {
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					Data++;
                	break;
            	case 'd':										  //十进制
                	d = va_arg(ap, int);
                	itoa(d, buf, 10);
                	for (s = buf; *s; s++) {
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					Data++;
                	break;
				default:
					Data++;
				    break;
			}		 
		}
		else USART_SendData(USARTx, *Data++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
	}
}

/******************************************************
		整形数据转字符串函数
        char *itoa(int value, char *string, int radix)
		radix=10 标示是10进制	非十进制，转换结果为0;  

	    例：d=-379;
		执行	itoa(d, buf, 10); 后
		
		buf="-379"							   			  
**********************************************************/
char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} 

static float x1,y1;
void setx(float posx)
{
	x1=posx;
}

 float getx(void)
 {
	 return x1;
 }
 
 void sety(float posy)
{
	y1=posy;
}

 float gety(void)                  
 {
	 return y1;
 }




 
