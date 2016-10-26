#include  "move.h"
#include "stm32f4xx.h"
#include "math.h"
#include "usart.h"
#include "stdlib.h"
#include "elmo.h"
#include "can.h"
#include "stm32f4xx_usart.h"
#include "other.h"
extern float real_x,real_y,real_angle;
extern uint8_t left[3],middle[3],right[3];
int ii_=0;
static	float v;


/*
函数： CloseLeftDoor
			CloseRightDoor
			OpenLeftDoor
			OpenRightDoor
作用：放球开关门 
参数：
*/
void CloseLeftDoor(void)
{
	PosCrl(3, 1, 2100);	
}
void CloseRightDoor(void)
{
	PosCrl(4, 1, -2150);	
}
void OpenLeftDoor(void)
{
	PosCrl(3, 1,-2100);
}

void OpenRightDoor(void)
{
	PosCrl(4, 1, +2100);
}

# define Kp 100.0f
# define Ki 0.0
# define Kd  0.0
static  float cl_angle(float ex, float act)
{

	static float err_sum = 0;
	static float err_last = 0;
	float err = 0;
	float err_vell = 0;
	err = ex - act;
	if (err > 180)
		err -= 360;
	else if (err < -180)
		err += 360;
	err_last = err;
	err_vell = err - err_last;
	err_sum += err;
	return (Kp*err + Ki*err_sum + Kd*err_vell);
}

/*全场被分为13*13个方格，每个方格边长为400mm*/
	static u8 map[13][13];    //用一个13*13的矩阵记录走过的方格，走过的区域标志为一，没走过的是0
static u8 col = 6, row =0;    //初始值为0行6列，即出发点所在处为原点
/*以上定义的两个变量都可以读取
  
   map记录着全场的行走情况
   row, col 记录着当前所处的方块区域
 */


static void record_track(float pos_x, float pos_y) //此处输入的参数posx, posy的坐标系可认为是陀螺仪发出来的数据把x,y轴方向变反后得到的坐标系
{
	if(pos_x>0)
	{
		while (pos_x/200 >0)
		{
			pos_x -=400;
			col +=1;
		}
	}
	else
	{
		while (pos_x/200 <0)
		{
			pos_x +=400;
			col -=1;
		}
	}
	
	while (pos_y/200 >0)
	{
		pos_y -= 400;
		row +=1;
	}
	
	map[row][col] = 1;
}

void print_map(void)
{
	u8 i, j;
	for(i=0; i<13;i++){
		USART_SendData(UART5, 0x0D);
		USART_SendData(UART5, 0x0A);
		for(j = 0; j < 13;j++){
			if(map[i][j] == 1)
				USART_SendData(UART5, 'O');
			else 
				USART_SendData(UART5, '.');
			USART_SendData(UART5, ' ');
		}
	}
}
	static float adjust,judge1,judge2,judge3,judge4,exangle_last=0,angleerr=0 ,exangle=0;
	static int num=16000,flag,count=0;

void velocitydecline( int di,int mark)			//过弯时减速
{
	angleerr=exangle-exangle_last;
	if(angleerr==90||angleerr==-270)
	  flag=1;
	if(flag==1)
	{
	  count++;
	}
	if(count== 1)	{num=10000;exangle=exangle_last;}
	if(count== 101 ) {num=15000;exangle=exangle;count=0;flag=0;}
	exangle_last=exangle;
//	USART_OUT(UART5, (uint8_t *)"  count %d", (int)count);
	}

void freedom(int bound12,int bound23,int bound34,int bound14,int num)							//自由行走
{

				   if((real_x<bound12&&real_y<bound14)||(real_x<bound12&&real_x>1600&&real_y<2175&&real_y>bound14))//区域一
					   exangle=0;
					 if((real_x>bound12&&real_y<bound23)||(real_x<bound12&&real_x>3200&&real_y<bound23&&real_y>2175))		
					   exangle=90;
	         if((real_x>bound34&&real_y>bound23)||(real_x<3200&&real_x>bound34&&real_y<bound23&&real_y>2725))
					   exangle=-180;
				   if((real_x<bound34&&real_y>bound14)||(real_x<1600&&real_x>bound34&&real_y<2725&&real_y>bound14))
						 exangle=-90; 

	adjust=cl_angle(exangle,real_angle);
	VelCrl(1,(num+adjust));
	VelCrl(2,-(num-adjust));
}
void freedom1(int i)							//自由行走
{	
	int s_c;
	int bound12,bound23,bound34,bound14;
	static int mark=1;
	static	float di=0;
	if(i==1) mark=3;
	switch(mark)
	{

		case 3:
					 bound12=1600,bound23=3300,bound34=3300,bound14=1600;
				   if((real_x>bound12&&real_y<bound14)||(real_x<3200&&real_x>bound12&&real_y<2175&&real_y>bound14))//区域一
				   {
					   exangle=-180;
					   if(real_y<2400&&real_x<2800&&real_x>2700)
					   {
						   if(di==0){mark=1;}
						   di=1;
				     }
				   }
					 if((real_x<bound12&&real_y<bound23)||(real_x<1600&&real_x>bound12&&real_y<bound23&&real_y>2175))		
					 {
					   num=15000;
					   exangle=90;
					 } //2
	         if((real_x<bound34&&real_y>bound23)||(real_x<bound34&&real_x>1600&&real_y<bound23&&real_y>2725))
					   exangle=0;
				   if((real_x>bound34&&real_y>bound14)||(real_x>3200&&real_x<bound34&&real_y<2725&&real_y>bound14))
				    {
						 exangle=-90;
						 di=0;
						}  
					 break;
		case 2:
											 bound12=1600,bound23=3300,bound34=3300,bound14=1600;
				   if((real_x>bound12&&real_y<bound14)||(real_x<3200&&real_x>bound12&&real_y<2175&&real_y>bound14))//区域一
				   {
					   exangle=-180;
					   if(real_y<2400&&real_x<2800&&real_x>2700)
					   {
						   if(di==0){mark=3;}
						   di=1;
				     }
				   }
					 if((real_x<bound12&&real_y<bound23)||(real_x<1600&&real_x>bound12&&real_y<bound23&&real_y>2175))		
					 {
					   num=15000;
					   exangle=90;
					 } //2
	         if((real_x<bound34&&real_y>bound23)||(real_x<bound34&&real_x>1600&&real_y<bound23&&real_y>2725))
					   exangle=0;
				   if((real_x>bound34&&real_y>bound14)||(real_x>3200&&real_x<bound34&&real_y<2725&&real_y>bound14))
				    {
						 exangle=-90;
						 di=0;
						}  
					 break;
		case 1:
											 bound12=1600,bound23=3300,bound34=3300,bound14=1600;
				   if((real_x>bound12&&real_y<bound14)||(real_x<3200&&real_x>bound12&&real_y<2175&&real_y>bound14))//区域一
				   {
					   exangle=-180;
					   if(real_y<2400&&real_x<2800&&real_x>2700)
					   {
						   if(di==0){mark=2;}
						   di=1;
				     }
				   }
					 if((real_x<bound12&&real_y<bound23)||(real_x<1600&&real_x>bound12&&real_y<bound23&&real_y>2175))		
					 {
					   num=15000;
					   exangle=90;
					 } //2
	         if((real_x<bound34&&real_y>bound23)||(real_x<bound34&&real_x>1600&&real_y<bound23&&real_y>2725))
					   exangle=0;
				   if((real_x>bound34&&real_y>bound14)||(real_x>3200&&real_x<bound34&&real_y<2725&&real_y>bound14))
				    {
						 exangle=-90;
						 di=0;
						}  
					 break;
  }
		if(real_x>4100)
		{
		if(fabs(real_angle-exangle)>75)
			num=15000-(real_x-3700)/400*10000;
		else 
			num=15000;
		}
		if(real_x<700)
		{
		if(fabs(real_angle-exangle)>75)
			num=15000-(1100-real_x)/400*10000;
		else 
			num=15000;
		}	
		if(real_y>4100)
		{
		if(fabs(real_angle-exangle)>75)
			num=15000-(real_y-3700)/400*10000;
		else 
			num=15000;
		}
		if(real_y<700)
		{
		if(fabs(real_angle-exangle)>75)
			num=15000-(1100-real_y)/400*10000;
		else 
			num=15000;
		}
					USART_OUT(UART5, (uint8_t *)"exangle %d\t", (int)exangle);
			if(num<5000)num=5000;
	velocitydecline(di,mark);
	adjust=cl_angle(exangle,real_angle);
	VelCrl(1,(num+adjust));
	VelCrl(2,-(num-adjust));
}


int send_CAN(void)
{
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=33;			// 标准标识符 
	TxMessage.ExtId=33;			// 设置扩展标示符 
	TxMessage.IDE=CAN_Id_Standard; 	// 标准帧
	TxMessage.RTR=CAN_RTR_Data;		// 数据帧
	TxMessage.DLC=1;				// 要发送的数据长度
	TxMessage.Data[0]=100;			          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)!=CAN_TxStatus_Ok)&&(i<0XFFF))i++;	
	if(i>=0XFFF) return 1;
	return 0;
}


int judge(float x,float y)
{
  static int jishu=0,num=0;
  int mark=0;
  static float a,b;
  static float xlast,ylast;
	a=x-xlast;
	b=y-ylast;
  if(fabs(a*a-b*b)<=1)
	jishu++;
  else 
	jishu=0;
  xlast=x;
  ylast=y;
  if(jishu==30)
	{
	mark=1;
	}
	if(mark==1&&((x>4500||y>4500||x<500||y<500)||(y<3175&&y>1625&&x<3700&&x>1100)))
		num++;
	if(num==2) 
		{
		  mark=2;num=0;
		}
	USART_OUT(UART5, (uint8_t *)"jishu %d\t", (int)jishu);
			USART_OUT(UART5, (uint8_t *)"xlast %d\t", (int)xlast);
			USART_OUT(UART5, (uint8_t *)"ylast %d\t", (int)ylast);
  return mark;  
}
void go_small(float x1,float y1,float x2,float y2,float advance1,float advance2,float V)
{
	static float exangle=20;
	static float adjust=0;
	static float distance;
  real_y=gety();
	if(ii_==0&&getx()>x2-advance1)
	{x2*=5;     ii_++;  exangle=90;}
	
	if(ii_==1&&gety()>y2-advance2)	
	{y2*=5;     ii_++;  exangle=180;}
	
	if(ii_==2&&getx()<x1+advance1)	
	{x1=-1000;  ii_++;  exangle=-90;}
	
	if(ii_==3&&gety()<y1+advance2)
	{y1=-1000;  ii_++;  exangle=0;}
	
	
//	if(ii_<=1)  distance=x2-real_x;
//	if(distance>360) distance=360;
//	if(distance<-360) distance=-360;
	if(ii_==4&&real_x>3000) ii_++;
   adjust=cl_angle(exangle,getAngle());	
	 VelCrl(2,-(V-adjust));
	 VelCrl(1, (V+adjust));
}

/////

void go_small2(float x1,float y1,float x2,float y2,float advance1,float advance2,float V)
{
	static float exangle=150;
	static float adjust=0;
  real_y=gety();
	if(ii_==0&&getx()<x1+advance1)
	{x2*=5;     ii_++;  exangle=90;}
	
	if(ii_==1&&gety()>y2-advance2)	
	{y2*=5;     ii_++;  exangle=0;}
	
	if(ii_==2&&getx()>x2-advance1)	
	{x1=-1000;  ii_++;  exangle=-90;}
	
	if(ii_==3&&gety()<y1+advance2)
	{y1=-1000;  ii_++;  exangle=180;}
	
	
//	if(ii_<=1)  distance=x2-real_x;
//	if(distance>360) distance=360;
//	if(distance<-360) distance=-360;
	if(ii_==4&&real_x>3000) ii_++;
   adjust=cl_angle(exangle,getAngle());	
	 VelCrl(2,-(V-adjust));
	 VelCrl(1, (V+adjust));
}

////
int geti(void)
{
	return ii_;
}
void cleari(void)
{
	ii_=0;
}

/////
void angleline(float exangle,float speed)
{
	if(fabs(real_angle-exangle)<2&&fabs(real_angle-exangle)>358)
		speed=15000;
	adjust=cl_angle(exangle,real_angle);
	VelCrl(1,(speed+adjust));
	VelCrl(2,-(speed-adjust));
}


 void smallmove(int mode)
{
	switch(mode)
	{
	case 0:
	{
		VelCrl(1,-3000);
		VelCrl(2,-1000);
	}break;
		case 1:
	{
	  VelCrl(1,0);
	  VelCrl(2,10000);
	}
	break;
	case 2:
	{
		VelCrl(1,0);
		VelCrl(2,8000);	
	}break;
	case 3:
	{
		VelCrl(1,-10000);
		VelCrl(2,10000);	
	}break;
		case 4:
	{
		VelCrl(1,0);
		VelCrl(2,-8000);	
	}break;
			case 5:
	{
		VelCrl(1,4000);
		VelCrl(2,-4000);	
	}break;
		case 6:
	{
		VelCrl(1,0);
		VelCrl(2,-4000);	
	}break;
		case 7:
	{
		VelCrl(1,0);
		VelCrl(2,0);	
	}break;
		case 8:
	{
		VelCrl(1,0);
		VelCrl(2,-3000);	
	}break;
		case 9:
	{
		VelCrl(1,-5000);
		VelCrl(2,5000);	
	}break;
		case 10:
	{
		VelCrl(1,4000);
		VelCrl(2,0);	
	}break;
}
}
static float ex1;
void setex(float posx)
{
	ex1=posx;
}

 float getex(void)
 {
	 return ex1;
 }

int max(uint8_t*area)
{
	int i, a=0;
	for (i = 0; i<7; i++)
	{
		if(i<3)
		{
		if (area[a] <= area[i + 1])
			a = i + 1;
	}
		else
		{
		if (area[a] < area[i + 1])
		a = i + 1;
	}
	}
	return a;
}

uint8_t area[7];
int judge_j(uint8_t*left,uint8_t*middle,uint8_t*right)
{
	int j=2,u;
	area[0]=left[0]+left[1]+left[2];
	area[1]=middle[0]+left[1]+left[2];
	area[2]=middle[0]+middle[1]+left[2];
	area[3]=middle[0]+middle[1]+middle[2];
	area[4]=right[0]+middle[1]+middle[2];
	area[5]=right[0]+right[1]+middle[2];
	area[6]=right[0]+right[1]+right[2];

			/********不要撞外墙去（离墙0到300）********/
	if(real_x>4500&&real_x<4800&&real_y>0&&real_y<4800)
	{
	if(real_angle>0&&real_angle<90-15)
	{
	area[3]=0;
	area[4]=0;
	area[5]=0;
	area[6]=0;
	}
	if(real_angle>90-15&&real_angle<90+0)
	{
	area[4]=0;
	area[5]=0;
	area[6]=0;
	}
		if(real_angle>90+0&&real_angle<90+15)
	{
	area[5]=0;
	area[6]=0;
	}
	if(real_angle>90+15&&real_angle<90+30)
	area[6]=0;
}
	if(real_x>0&&real_x<4800&&real_y>4300&&real_y<4800)
	{
	if(real_angle>90&&real_angle<180-15)
	{
	area[3]=0;
	area[4]=0;
	area[5]=0;
	area[6]=0;
	}
	if(real_angle>180-15&&real_angle<180+0)
	{
	area[4]=0;
	area[5]=0;
	area[6]=0;
	}
		if(real_angle>-180+0&&real_angle<-180+15)
	{
	area[5]=0;
	area[6]=0;
	}
	if(real_angle>-180+15&&real_angle<-180+30)
	area[6]=0;
}
		if(real_x>0&&real_x<300&&real_y>0&&real_y<4800)
	{
		if(real_angle>0-180&&real_angle<-90-15)
	{
	area[3]=0;
	area[4]=0;
	area[5]=0;
	area[6]=0;
	}
	if(real_angle>-90-15&&real_angle<-90+0)
	{
	area[4]=0;
	area[5]=0;
	area[6]=0;
	}
		if(real_angle>-90+0&&real_angle<-90+15)
	{
	area[5]=0;
	area[6]=0;
	}
	if(real_angle>-90+15&&real_angle<-90+30)
	area[6]=0;
}
		/********不要撞外墙去（离墙700到300）********/
		if(real_x>0&&real_x<4800&&real_y>0&&real_y<300)
	{
	if(real_angle>0-90&&real_angle<0-15)
	{
	area[3]=0;
	area[4]=0;
	area[5]=0;
	area[6]=0;
	}
	if(real_angle>0-15&&real_angle<0+0)
	{
	area[4]=0;
	area[5]=0;
	area[6]=0;
	}
		if(real_angle>0+0&&real_angle<0+15)
	{
	area[5]=0;
	area[6]=0;
	}
	if(real_angle>0+15&&real_angle<0+30)
	area[6]=0;
}
	if(real_x>0&&real_x<4800&&real_y>4100&&real_y<4500)//别忘了四个边角
	{
	if(real_angle>90-30&&real_angle<90+30)
	{
	area[2]=0;
	area[3]=0;
	area[4]=0;
	}
	if(real_angle>90+30&&real_angle<90+60)
	{
	area[3]=0;
	area[4]=0;
	area[5]=0;
	}
		if(real_angle>90+60&&real_angle<90+90)
	{
	area[4]=0;
	area[5]=0;
	area[6]=0;
	}
}
		if(real_x>0&&real_x<4800&&real_y>300&&real_y<700)
	{
	if(real_angle>-90-30&&real_angle<-90+30)
	{
	area[2]=0;
	area[3]=0;
	area[4]=0;
	}
	if(real_angle>-90+30&&real_angle<-90+60)
	{
	area[3]=0;
	area[4]=0;
	area[5]=0;
	}
		if(real_angle>-90+60&&real_angle<-90+90)
	{
	area[4]=0;
	area[5]=0;
	area[6]=0;
	}
}
			if(real_x>4100&&real_x<4500&&real_y>0&&real_y<4800)
	{
	if(real_angle>0-30&&real_angle<0+30)
	{
	area[2]=0;
	area[3]=0;
	area[4]=0;
	}
	if(real_angle>0+30&&real_angle<0+60)
	{
	area[3]=0;
	area[4]=0;
	area[5]=0;
	}
		if(real_angle>0+60&&real_angle<0+90)
	{
	area[4]=0;
	area[5]=0;
	area[6]=0;
	}
}
				if(real_x>300&&real_x<700&&real_y>0&&real_y<4800)
	{
	if(real_angle>180-30||real_angle<-180+30)
	{
	area[2]=0;
	area[3]=0;
	area[4]=0;
	}
	if(real_angle>-180+30&&real_angle<-180+60)
	{
	area[3]=0;
	area[4]=0;
	area[5]=0;
	}
		if(real_angle>-180+60&&real_angle<-180+90)
	{
	area[4]=0;
	area[5]=0;
	area[6]=0;
	}
}
	/********外墙拐晚不判断********/
if((real_x>0&&real_x<700&&real_y>0&&real_y<700)||(real_x>0&&real_x<700&&real_y>4100&&real_y<4800)||(real_x>4100&&real_x<4800&&real_y>0&&real_y<700)||(real_x>4100&&real_x<4800&&real_y>4100&&real_y<4800))
	{
	area[4]=0;
	area[5]=0;
	area[6]=0;
	area[1]=0;
	area[2]=0;
	area[3]=0;
	area[0]=0;		
}
		/********内圈********/
if((real_y<2125&&real_y>1900&&real_x<2950&&real_x>1600)||(real_y<2525&&real_y>1975&&real_x<3350&&real_x>3200)||(real_y<2775&&real_y>2275&&real_x<1600&&real_x>1350)||(real_y<3925&&real_y>2675&&real_x<3300&&real_x>1850))//下区域
	{
	area[0]=0;
	area[1]=0;
	area[2]=0;
}

//  if((real_y<2125&&real_y>1725&&real_x<2950&&real_x>1600)||(real_y<2425&&real_y>2175&&real_x<3600&&real_x>3200)||(real_y<2675&&real_y>1875&&real_x<1600&&real_x>1200))//2675   2125
//  {
//	  if(getAngle()<(exangle+3)) left=0;	
//	  if(fabs(getAngle()-exangle)>5)
//	  middle=0;
//  }
//  if(real_y<3075&&real_y>2675&&real_x<3200&&real_x>1850)
//  {	
// 	  if(fabs(getAngle()-exangle)>5)
//	  middle=0;
//  }
//							//这是进入内圈看到墙要往外拐了		设定左(右)没球
//  if(real_y>3800||real_y<1000||real_x>3800||real_x<1000)
//  {
//		if((real_y>3801&&real_x>3801)||(real_y>3801&&real_x<1001)||(real_y<1001&&real_x<1001)||(real_y<1000&&real_x>3801))
//		{right=0;middle=0;}
//		else if((real_y<4300&&real_y>500&&real_x<4300&&real_x>500)&&fabs(getAngle()-exangle)<5)
//		right=right;
//		else
//		right=0;
//		if((real_y<4300&&real_y>500&&real_x<4300&&real_x>500)&&fabs(getAngle()-exangle)<70)
//		middle=middle;
//		else
//		middle=0;
//		if((real_y>4300||real_y<500||real_x>4300||real_x<500)&&fabs(getAngle()-exangle)>5)
//		middle=0;
//  }
	j=max(area);
	if(middle==0&&left==0&&right==0)
	j= 7;
		if((real_x<1100)||(real_y<1100)||(real_x>3700))
		{
		if(fabs(real_angle-exangle)>65)
		j=7;
		}	
		if(real_y>3700)
		{
		if(fabs(real_angle-exangle)>295)
		j=7;
		}
		if(real_y<3325&&real_y>1475&&real_x<3700&&real_x>1100)
{
		if((fabs(real_angle-exangle)>295)||(fabs(real_angle-exangle)>65))
			j=7;
}
//			USART_OUT(UART5, (uint8_t *)"  area0=%d",area[0]);	
//			USART_OUT(UART5, (uint8_t *)"  area1=%d",area[1]);
//			USART_OUT(UART5, (uint8_t *)"  area2=%d",area[2]);
//			USART_OUT(UART5, (uint8_t *)"  area3=%d",area[3]);
//			USART_OUT(UART5, (uint8_t *)"  area4=%d",area[4]);
//			USART_OUT(UART5, (uint8_t *)"  area5=%d",area[5]);
//			USART_OUT(UART5, (uint8_t *)"  area6=%d\r\n",area[6]);
	return j;
}
static int nx=1,ji=0,nx0=1,nx1=1,nx2=1,nx4=1,nx5=1,nx6=1;
void findball(int j)
{
	int u=0;
	if(j==7)			{	nx0=1,nx1=1,nx2=1,nx4=1,nx5=1,nx6=1,nx=1;}
		if(j==0&&nx==1&&nx0==1)   
		{
nx0=1,nx1=0,nx2=0,nx4=0,nx5=0,nx6=0;
			exangle+=0.5;
			adjust=cl_angle(exangle,getAngle());
			VelCrl(1,(15000+adjust));
			VelCrl(2,-(15000-adjust));
		}
		if(j==1&&nx==1&&nx1==1) 
		{
nx0=0,nx1=1,nx2=0,nx4=0,nx5=0,nx6=0;
			exangle+=0.5;
			adjust=cl_angle(exangle,getAngle());
			VelCrl(1,(15000+adjust));
			VelCrl(2,-(15000-adjust));
		}
				if(j==2&&nx==1&&nx2==1) 
		{
nx0=0,nx1=0,nx2=1,nx4=0,nx5=0,nx6=0;
			exangle+=0.4;
			adjust=cl_angle(exangle,getAngle());
			VelCrl(1,(15000+adjust));
			VelCrl(2,-(15000-adjust));
		}
		if(j==4&&nx==1&&nx4==1)  
		{
nx0=0,nx1=0,nx2=0,nx4=1,nx5=0,nx6=0;
			exangle-=0.4;
			adjust=cl_angle(exangle,getAngle());
			VelCrl(1,(15000+adjust));
			VelCrl(2,-(15000-adjust));
		}
				if(j==5&&nx==1&&nx5==1)  
		{
nx0=0,nx1=0,nx2=0,nx4=0,nx5=1,nx6=0;
			exangle-=0.5;
			adjust=cl_angle(exangle,getAngle());
			VelCrl(1,(15000+adjust));
			VelCrl(2,-(15000-adjust));
		}
		if(j==6&&nx==1&&nx6==1)  
		{
nx0=0,nx1=0,nx2=0,nx4=0,nx5=0,nx6=1;
			exangle-=0.5;
			adjust=cl_angle(exangle,getAngle());
			VelCrl(1,(15000+adjust));
			VelCrl(2,-(15000-adjust));
		}
		if(j==3) nx=0;
		if(nx==0)   
		{
			if(ji<30)
			{  
				adjust=cl_angle(exangle,getAngle());
				VelCrl(1,(15001+adjust));
				VelCrl(2,-(15001-adjust));
			}
			if(ji>30)  
			{
				nx=1;j=3;ji=0;
			}
			nx0=1,nx1=1,nx2=1,nx4=1,nx5=1,nx6=1;
			ji++;
		}
		setnx(nx);
//	USART_OUT(UART5, (uint8_t *)" j=%d\r\n", (int)j);
//	USART_OUT(UART5, (uint8_t *)"  nx=%d", (int)nx);
//		USART_OUT(UART5, (uint8_t *)"  area0=%d",area[0]);	
//			USART_OUT(UART5, (uint8_t *)"  area1=%d",area[1]);
//			USART_OUT(UART5, (uint8_t *)"  area2=%d",area[2]);
//			USART_OUT(UART5, (uint8_t *)"  area3=%d",area[3]);
//			USART_OUT(UART5, (uint8_t *)"  area4=%d",area[4]);
//			USART_OUT(UART5, (uint8_t *)"  area5=%d",area[5]);
//			USART_OUT(UART5, (uint8_t *)"  area6=%d\r\n",area[6]);	
//	USART_OUT(UART5, (uint8_t *)" exangle	%d\r\n", (int)exangle);
//	USART_OUT(UART5, (uint8_t *)" getAngle()		%d", (int)getAngle());

	}

int nxset;
 void setnx(int nx)
{
	nxset=nx;
}

	int	getnx()
	{
	return nxset;
	}
	
/**
	前进的角度闭环函数
	期望角度，速度，p参数
**/
void forward_anglemove(int expectangle,u16 speed, u8 proportion)
{
					adjust = proportion/100*cl_angle(expectangle, real_angle);//cl_angle初始p为100
					VelCrl(1, (speed + adjust));		//一轮正为向前转
					VelCrl(2, -(speed - adjust));		//二轮负为向前转
}
/**
	后退的角度闭环函数
	期望角度，速度，p参数
**/
void back_anglemove(int expectangle,u16 speed, u8 proportion)
{
					adjust = proportion/100*cl_angle(expectangle, real_angle);//cl_angle初始p为100
					VelCrl(1, -(speed - adjust));		//一轮正为向前转
					VelCrl(2, (speed + adjust));		//二轮负为向前转
}

