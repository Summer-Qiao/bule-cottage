#include  <includes.h>
#include  <app_cfg.h>
#include "misc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "timer.h"
#include "gpio.h"
#include "usart.h"
#include "can.h"
#include "elmo.h"
#include "action_math.h"
#include "GET_SET.h"
#include "stm32f4xx_usart.h"
#include "math.h"
#include "stm32f4xx_it.h"
#include  "move.h"
#include "collectball.h"
#include "other.h"

//////////////////Area of defining semaphore////////////////////////
OS_EVENT 		*PeriodSem;
void App_Task()
{
	CPU_INT08U  os_err;
	os_err = os_err; /* prevent warning... */

					 /******************Create Semaphore***********************/
	PeriodSem = OSSemCreate(0);

	/******************Create Task**************************/
	os_err = OSTaskCreate((void(*)(void *)) ConfigTask,					//Initial Task
		(void          *)0,
		(OS_STK        *)&App_ConfigStk[Config_TASK_START_STK_SIZE - 1],
		(INT8U)Config_TASK_START_PRIO);


	os_err = OSTaskCreate((void(*)(void *)) WalkTask,
		(void          *)0,
		(OS_STK        *)&WalkTaskStk[Walk_TASK_STK_SIZE - 1],
		(INT8U)Walk_TASK_PRIO);
}




/*函数的申明*/
extern int16_t colorgroundright;
static int16_t getcha(int16_t colorgroundright);
static float cl_angle1(float ex, float act);
static float cl_angle(float ex, float act);

char colorinit;		//初始化颜色，用于分辨红蓝场
void ConfigTask(void)
{
	CPU_INT08U  os_err;

	os_err = os_err;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	KeyInit();
	CAN_Config(CAN1, 500, GPIOB, GPIO_Pin_8, GPIO_Pin_9);
	elmo_Enable(1);
	elmo_Enable(2);
	Vel_cfg(1, 30000, 30000);
	Vel_cfg(2, 30000, 30000);
	elmo_Enable(3);
	elmo_Enable(4);
	Pos_cfg(3, 30000, 30000, 15000);
	Pos_cfg(4, 30000, 30000, 15000);
	colorinit = colorgroundright;
	/*串口初始化*/
	USART1_Init(115200);
	UART5_Init(115200);
	GPIO_Init_Pins(GPIOE, GPIO_Pin_7, GPIO_Mode_IN);
	GPIO_Init_Pins(GPIOE, GPIO_Pin_12, GPIO_Mode_IN);
	GPIO_Init_Pins(GPIOB, GPIO_Pin_0, GPIO_Mode_IN);
	GPIO_Init_Pins(GPIOB, GPIO_Pin_15, GPIO_Mode_OUT);
	CloseLeftDoor();
	CloseRightDoor();
	/*********************TIMER INIT*******************************/
	TIM_Init(TIM2, 999, 839, 0, 0);
	TIM4_Pwm_Init(2000, 839);
	TIM_Delayms(TIM5, 15000);
	TIM_SetCompare1(TIM4, 0.09 * 2000);
	TIM_SetCompare3(TIM4, 0.09 * 2000);
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
	TIM_Delayms(TIM5, 3000);
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	OSTaskSuspend(OS_PRIO_SELF);
}static int16_t getcha2(int16_t colorgroundright);
void color(void);  //扫墙过渡，矫正区上下晃，撞墙的调解不过来，调节过来不走，顺序，躲避车，判断有没有撞车
static int status = 1, jishu5 = 0, jishu6 = 0;
extern int TIME_s; static int bound12, bound23, bound34, bound14, jishuq = 0;
extern int16_t colorgroundleft;
static int16_t getcha(int16_t colorgroundright);
static int target = 1, count = 0;
static float a_, b_, a1, b1, speed1, speed2;
float  real_angle;
static int cnt = 0, di = 0, jishu = 0, jishuend = 0, counti = 0, righty = 0, admit = 1, permit = 0;
static int jishu, hehe = 1;
static int flag_for_vel = 1, i = 0, speeed;
float  real_x, real_y;
static float rvel = 0, lvel = 0, fail = 0, jinqushibai = 0;
static int  cha, asd, angle0, small = 1, sub = 0;
static int step_to_warehouse = 1, judge_y_correct = 0;
static float distance_to_warehouse = 0;
static float righta = 0, rightb = 0;
static float Velnow;
static float rad3 = 0, rad4 = 90, rad5 = -180, rad6 = 180, rad7 = -90, lasta1 = 0, lastb1 = 0, rad1 = 0;
extern uint8_t left[3], middle[3], right[3];
extern int ii_;
extern u8 whiteball, blackball;
void WalkTask(void)
{
	static int  statustask = 1, mark1 = 0, nn = 1;
	static int adjust = 0, jishufor = 0, jishuforfang = 0, num, flag1 = 0;
	static float exangle_old = 0;
	static float exangle = 0;
	static int area, area_last, already = 0;
	static float exangle_add = 0;
	static int jishuend, targetq = 1, numcircle = 0, dii = 0;
	static float x_crash, y_crash, step11 = 1, markcrash = 0;
	static int kaqiu = 0, cnt1 = 0;
	static int i = 0, markoo = 0, order = 0;
	static int biaoi = 1, statustask_last;
	static float mubiao = 2400;
	static int markii = 0, jishu4, crash = 0, cnt2 = 0, em = 0;
	static int flag3 = 0, saoqiang = 1;
	static int mark = 0, j = 7, nx = 1;
	static float x0, y0, distance = 0, error = 0;
	static int markstop = 0, flag = 0, step = 1, smallstep = 1, situation = 0, defend = 1;
	CPU_INT08U  os_err;
	os_err = os_err;
	OSSemSet(PeriodSem, 0, &os_err);
	while (PhoElec_R == 0 && PhoElec_L == 0 && PhoElec_RF == 0);
	if (PhoElec_R == 1) statustask = 1;
	else if (PhoElec_RF == 1) statustask = 13;
	USART_OUT(USART1, (uint8_t *)"ACT0");//给陀螺仪发送一个数据
	TIME_s = 0;
	while (1)
	{
		OSSemPend(PeriodSem, 0, &os_err);
		a_ = getx();
		b_ = gety();
		if (real_y > 500) flag3 = 1;
		a1 = a_*cos(rad1*0.017453292) - b_*sin(rad1*0.017453292);
		b1 = a_*sin(rad1*0.017453292) + b_*cos(rad1*0.017453292);
		real_x = a1 - lasta1;
		real_y = b1 - lastb1;
		Velnow = getv();
		real_angle = getAngle() + rad1;
		if (real_angle > 180) real_angle -= 360;
		if (real_angle < -180)real_angle += 360;
		if (real_y < 2400 && real_x < 1600 && real_y>2000)
		{
			if (dii == 0)
			{
				numcircle++;
				dii = 1;
			}
		}

		if (real_y < 2400 && real_x < 3200 && real_x>3000)	 dii = 0;
		if (numcircle == 5) { numcircle = 0; statustask = 7; cnt = 0; cnt1 = 0; cnt2 = 0; step_to_warehouse = 1; step = 0; step = 1; targetq = 1; count = 0; biaoi = 1; defend = 1; }
		if (statustask == 5)
		{
			if (real_y < 2400 && real_x < 1600 && real_y>2000)
			{
				if (di == 0)
				{
					switch (nn)
					{
					case 1: nn = 2;
						break;
					case 2:  nn = 1;
						break;
					}
					jishu++;
					di = 1;
				}
			}

			if (real_y < 2400 && real_x < 3200 && real_x>3000)	 di = 0;
			if (jishu >= 3) { statustask = 4; targetq = 7; asd = 2; jishu = 0; nn = 1; }
		}
		/*********judge situation*******/
		{
			if (fabs(Velnow) == 0)
			{
				cnt++; cnt2++;
			}
			else
			{
				cnt = 0; cnt2 = 0;
			}
			if (fabs(getv1()) == 0)
				cnt1++;
			else
				cnt1 = 0;
			if (cnt >= 15)
				markstop = 1;//no speed
			else
				markstop = 0;
			if (cnt > 200)
				markstop = 2;
			distance = sqrt((real_x - x0)*(real_x - x0) + (real_y - y0)*(real_y - y0));
			error = fabs(real_angle - exangle);
			if (real_x > 4300 && real_y < 4300)//right
			{
				if (NOSPEED)
				{
					if (real_angle <= 120 && real_angle >= -60)//straght
						situation = 1;
					else                      //oblique
						situation = 2;
					if (real_y < 400)
					{
						if (real_angle <= 90 && real_angle >= 0)
							situation = 3;				//正斜卡
						if (real_angle <= 180 && real_angle >= 90)
							situation = 4;				//朝外卡
						if (real_angle <= -90 && real_angle >= -180)
							situation = 5;				//反斜卡
						if (real_angle <= 0 && real_angle >= -90)
							situation = 6;				//朝内卡
					}
				}
			}
			else if (real_x > 500 && real_y > 4300)//up
			{
				if (NOSPEED)
				{
					if ((real_angle <= 180 && real_angle >= -30) || (real_angle < -150))//straght
						situation = 1;
					else                          //oblique
						situation = 2;
					if (real_x > 4400)
					{
						if (real_angle <= 90 && real_angle >= 0)
							situation = 6;
						if (real_angle <= 180 && real_angle >= 90)
							situation = 3;
						if (real_angle <= -90 && real_angle >= -180)
							situation = 4;
						if (real_angle <= 0 && real_angle >= -90)
							situation = 5;
					}
				}
			}
			else if (real_x < 500 && real_y > 500)//left
			{
				if (NOSPEED)
				{
					if (real_angle <= -30 || real_angle >= 60)//straght
						situation = 1;
					else                          //oblique
						situation = 2;
					if (real_y > 4400)
					{
						if (real_angle <= 90 && real_angle >= 0)
							situation = 5;
						if (real_angle <= 180 && real_angle >= 90)
							situation = 6;
						if (real_angle <= -90 && real_angle >= -180)
							situation = 3;
						if (real_angle <= 0 && real_angle >= -90)
							situation = 4;
					}
				}
			}
			else if (real_x < 4300 && real_y < 500)//down
			{
				if (NOSPEED)
				{
					if ((real_angle <= 30 && real_angle >= -180) || (real_angle>150))//straght
						situation = 1;
					else                          //oblique
						situation = 2;
					if (real_x < 400)
					{
						if (real_angle <= 90 && real_angle >= 0)
							situation = 4;				//正斜卡
						if (real_angle <= 180 && real_angle >= 90)
							situation = 5;				//朝外卡
						if (real_angle <= -90 && real_angle >= -180)
							situation = 6;				//反斜卡
						if (real_angle <= 0 && real_angle >= -90)
							situation = 3;				//朝内卡
					}
				}
			}
			else if (real_x < 3900 && real_x>900 && real_y < 3200 && real_y > 1400)
			{
				if (NOSPEED)
				{
					situation = 9;
				}
			}
			else if (NOSPEED)
			{
				situation = 10;
			}

			if (admit == 1)
			{
				if (statustask != 8 && statustask != 11 && statustask != 6 && statustask != 4)
				{
					x0 = real_x;
					y0 = real_y;
				}
				step = 1;
			}
			if (jishuend > 500)
			{
				if (step11 == 1)
				{
					step = 1;
					step11 = 0;
				}
				situation = 11;
			}

		}
		//		if((PhoElec_LF==1||PhoElec_RF==1)&&situation!=12)
		//			situation=12;
		if ((permit == 0) && (statustask == 15 || statustask == 10 || statustask == 8 || statustask == 9 || statustask == 4 || statustask == 11 || statustask == 6 || statustask == 20))
			situation = 0;
		if (cnt2 >= 250 && statustask != 15)
		{
			step_to_warehouse = 1; step = 0; step = 1; targetq = 1; biaoi = 1; defend = 1;
			situation = 0; statustask = 7; em = 1; count = 0;
			VelCrl(2, 8000);
			VelCrl(1, -8000);
			delay_ms(400);
			cnt2 = 0; cnt1 = 0; cnt = 0;
		}
		if (flag3 != 1)
			situation = 0;
		if (situation == 0) jishuend = 0;
		switch (situation)
		{
		case 0:
		{
			if (crash == 9)
				markcrash = 0;
			if (crash == 10)
			{
				if (markcrash == 0)
				{
					statustask_last = statustask;
					if (statustask == 8 || statustask == 11 || statustask == 9 || statustask == 6)
					{
						statustask_last = 7;
					}
					statustask = 6; mark = 0;
					biaoi = 1;
					markcrash = 1;
					crash = 0;
					fail++;
				}
			}
			if (statustask == 1)                 //
			{
				color();
				go_small(1390, 1915, 3410, 2700, 800, 700, 26000);
				if (geti() >= 4) { statustask = 2; cleari(); }
			}
			else if (statustask == 2)            //zhong
			{
				color();
				go_small(1290, 1915, 3410, 2800, 700, 700, 26000);
				if (geti() >= 4) { statustask = 1; order = 1; cleari(); }
			}
			else if (statustask == 3)            //中圈	
			{
				color();
				switch (nn)
				{
				case 1:
					go_small(1390, 1915, 3410, 2705, 800, 800, 20000);
					if (geti() >= 4) { nn = 2; cleari(); }
					break;
				case 2:
					go_small(1390, 1915, 3410, 2700, 800, 800, 25000);
					if (geti() >= 4) { statustask = 7; order = 1; cleari(); nn = 1; }
					break;
				}
			}
			else if (statustask == 4)            //扫墙			
			{
				color();
				switch (targetq)
				{
				case 1:
				{
					angleline(-0.5, 18000);
					if (real_x > 4180) { targetq = 2; asd = 2; }if (real_x > 3100) speeed = 10000;
					if (cnt1 == 10 && real_x > 4000)
					{
						VelCrl(1, -8000);
						VelCrl(2, 8000);
						delay_ms(200);
						targetq = 2;
						asd = 2;
						cnt1 = 0;
					}
					if ((PhoElec_RF == 1 || PhoElec_LF == 1 || markstop == 1) && real_x < 4000)
					{
						flag = 1;
					}
					if (flag == 0)
					{
						x0 = real_x;
						y0 = real_y;
						angle0 = real_angle;
					}
					if (flag == 1)
					{
						VelCrl(1, -5000);
						VelCrl(2, 5000);
						if (distance > 200)
						{
							VelCrl(1, 5000);
							VelCrl(2, 5000);
						}
						if (fabs(angle0 - real_angle) > 70)
						{
							VelCrl(1, 5000);
							VelCrl(2, -5000);
							delay_ms(1000);
							flag = 0;
						}

					}
					break;
				}
				case 2:
				{
					if (asd == 2)asd = 2;
					if ((real_angle > 175 || real_angle < -175) && asd == 6)
						asd = 3;
					if (((KEY_R == 1 && KEY_L == 1) || cnt == 10) && asd == 3)
					{
						asd = 7;
						if ((KEY_R == 1 && KEY_L == 1))
						{
							if (getAngle() > 0)
								rad1 = -(getAngle() - 180);
							else
								rad1 = -(getAngle() + 180);
						}
					}
					switch (asd)
					{
					case 2:
						smallmove(2);
						if (real_angle < 93 && real_angle>80)
							asd = 5;
						x0 = real_x;
						y0 = real_y;
						break;
					case 5:
						VelCrl(1, 7000);
						VelCrl(2, -7000);
						jishu++;
						if (distance > 250 && jishu > 2)
						{
							asd = 6;
							jishu = 0;
						}
						if (jishu == 100 && distance < 100)
						{
							situation = 11;
							permit = 1;
							step = 3;
							cnt = -10;
							kaqiu = 0;
							jishu = 0;
						}
						break;
					case 6:
						smallmove(1);
						break;
					case 3:
					{
						adjust = cl_angle(-180, real_angle);
						VelCrl(1, -(10000 - adjust));
						VelCrl(2, (10000 + adjust));
					}
					break;
					case 7:
						VelCrl(1, 5000);
						VelCrl(2, -5000);
						delay_ms(200);
						asd = 4;
						break;
					case 4:
						smallmove(4);
						cnt1 = 0;
						break;
					}
					if (real_angle < 91 && real_angle>87 && asd == 4)
					{
						targetq = 3;
						asd = 2;
					}
					if (cnt1 == 35 && asd == 4)
					{
						VelCrl(1, -5000);
						VelCrl(2, 5000);
						delay_ms(500);
						cnt1 = 0;
						jishu++;
						targetq = 3;
						asd = 2;
					}
					if (cnt1 == 35 && asd == 3)
					{
						VelCrl(1, 5000);
						VelCrl(2, -5000);
						delay_ms(500);
						cnt1 = 0;
					}
					if (cnt1 == 35 && asd == 6)
					{
						VelCrl(1, 5000);
						VelCrl(2, -5000);
						delay_ms(500);
						cnt1 = 0;
					}
					if (cnt1 == 10 && asd != 4 && asd != 3 && asd != 6)
					{
						VelCrl(1, -5000);
						VelCrl(2, 5000);
						delay_ms(500);
						cnt1 = 0;
					}
					break;
				}
				case 3:
				{
					angleline(89.5, 18000);
					if (real_y > 4180) { targetq = 4; }if (real_y > 3100) speeed = 10000;
					if (cnt1 == 10 && real_y > 4000)
					{
						VelCrl(1, -8000);
						VelCrl(2, 8000);
						delay_ms(200);
						targetq = 2;
						cnt1 = 0;
					}
					if ((PhoElec_RF == 1 || PhoElec_LF == 1 || markstop == 1) && real_y < 4000)
					{
						flag = 1;
					}
					if (flag == 0)
					{
						x0 = real_x;
						y0 = real_y;
						angle0 = real_angle;

					}
					if (flag == 1)
					{
						VelCrl(1, -5000);
						VelCrl(2, 5000);
						if (distance > 200)
						{
							VelCrl(1, 5000);
							VelCrl(2, 5000);
						}
						if (fabs(angle0 - real_angle) > 70)
						{
							VelCrl(1, 5000);
							VelCrl(2, -5000);
							delay_ms(1000);
							flag = 0;
						}

					}
					break;
				}
				case 4:
				{
					if (asd == 2)
						asd = 2;
					if (real_angle<-90 && real_angle>-97 && asd == 6)
						asd = 3;
					if (((KEY_R == 1 && KEY_L == 1) || cnt == 10) && asd == 3)
					{
						asd = 7;
						if (KEY_R == 1 && KEY_L == 1)
							rad1 = -90 - getAngle();
					}
					switch (asd)
					{
					case 2:
						smallmove(2);
						if (real_angle < -178 || real_angle>170)
							asd = 5;
						x0 = real_x;
						y0 = real_y;
						break;
					case 5:
						VelCrl(1, 7000);
						VelCrl(2, -7000);
						jishu++;
						if (distance > 250 && jishu > 2)
						{
							asd = 6;
							jishu = 0;
						}
						if (jishu == 100 && distance < 100)
						{
							situation = 11;
							permit = 1;
							step = 3;
							cnt = -10;
							kaqiu = 0;
							jishu = 0;
						}
						break;
					case 6:
						smallmove(1);
						break;
					case 3:
						adjust = cl_angle(-90, real_angle);
						VelCrl(1, -(10000 - adjust));
						VelCrl(2, (10000 + adjust));
						break;
					case 7:
						VelCrl(1, 5000);
						VelCrl(2, -5000);
						delay_ms(200);
						asd = 4;
						break;
					case 4:
						smallmove(4);
						cnt1 = 0;
						break;
					}
					if ((real_angle < -178 || real_angle>177) && asd == 4)
					{
						targetq = 5;
						asd = 2;

					}
					if (cnt1 == 35 && asd == 4)
					{
						VelCrl(1, -5000);
						VelCrl(2, 5000);
						delay_ms(500);
						cnt1 = 0;

						targetq = 5;
						asd = 2;
					}
					if (cnt1 == 35 && asd == 3)
					{
						VelCrl(1, 5000);
						VelCrl(2, -5000);
						delay_ms(500);
						cnt1 = 0;
					}
					if (cnt1 == 35 && asd == 6)
					{
						VelCrl(1, 5000);
						VelCrl(2, -5000);
						delay_ms(500);
						cnt1 = 0;
					}
					if (cnt1 == 10 && asd != 4 && asd != 3 && asd != 6)
					{
						VelCrl(1, -5000);
						VelCrl(2, 5000);
						delay_ms(500);
						cnt1 = 0;
					}
					break;
				}
				case 5:
				{
					angleline(179.5, 18000);
					if (real_x < 620) { targetq = 6; asd = 2; }if (real_x < 1500) speeed = 10000;
					if (cnt1 == 10 && real_x < 820)
					{
						VelCrl(1, -5000);
						VelCrl(2, 5000);
						delay_ms(500);
						targetq = 6; asd = 2;
						cnt1 = 0;
					}
					if ((PhoElec_RF == 1 || PhoElec_LF == 1 || markstop == 1) && real_x>820)
					{
						flag = 1;
					}
					if (flag == 0)
					{
						x0 = real_x;
						y0 = real_y;
						angle0 = real_angle;

					}
					if (flag == 1)
					{
						VelCrl(1, -5000);
						VelCrl(2, 5000);
						if (distance > 200)
						{
							VelCrl(1, 5000);
							VelCrl(2, 5000);
						}
						if (fabs(angle0 - real_angle) > 70)
						{
							VelCrl(1, 5000);
							VelCrl(2, -5000);
							delay_ms(1000);
							flag = 0;
						}

					}
					break;
				}
				case 6:
				{
					if (asd == 2)asd = 2;
					if (real_angle<2 && real_angle>-2 && asd == 6)
						asd = 3;
					if (((KEY_R == 1 && KEY_L == 1) || cnt == 10) && asd == 3)
					{
						asd = 7;
						if (KEY_R == 1 && KEY_L == 1)
							rad1 = 0 - getAngle();
					}
					switch (asd)
					{
					case 2:
						smallmove(2);
						if (real_angle<-88 && real_angle>-100)
							asd = 5;
						x0 = real_x;
						y0 = real_y;
						break;
					case 5:
						VelCrl(1, 7000);
						VelCrl(2, -7000);
						jishu++;
						if (distance > 250 && jishu > 2)
						{
							asd = 6;
							jishu = 0;
						}
						if (jishu == 100 && distance < 100)
						{
							situation = 11;
							permit = 1;
							step = 3;
							cnt = -10;
							kaqiu = 0;
							jishu = 0;
						}
						break;
					case 6:
						smallmove(1);
						break;
					case 3:
						adjust = cl_angle(0, real_angle);
						VelCrl(1, -(10000 - adjust));
						VelCrl(2, (10000 + adjust));
						break;
					case 7:
						VelCrl(1, 5000);
						VelCrl(2, -5000);
						delay_ms(200);
						asd = 4;
						break;
					case 4:
						smallmove(4);
						cnt1 = 0;
						break;
					}
					if (real_angle<-88 && real_angle>-92 && asd == 4)
					{
						targetq = 7;
						asd = 2;
					}
					if (cnt1 == 35 && asd == 4)
					{
						VelCrl(1, -5000);
						VelCrl(2, 5000);
						delay_ms(500);
						cnt1 = 0;

						targetq = 7;
						asd = 2;
					}
					if (cnt1 == 35 && asd == 3)
					{
						VelCrl(1, 5000);
						VelCrl(2, -5000);
						delay_ms(500);
						cnt1 = 0;
					}
					if (cnt1 == 35 && asd == 6)
					{
						VelCrl(1, 5000);
						VelCrl(2, -5000);
						delay_ms(500);
						cnt1 = 0;
					}
					if (cnt1 == 10 && asd != 4 && asd != 3 && asd != 6)
					{
						VelCrl(1, -5000);
						VelCrl(2, 5000);
						delay_ms(500);
						cnt1 = 0;
					}
					break;
				}
				case 7:
				{
					if (real_y < 4000 && real_y>3000)  i = 2;
					angleline(-90.5, 18000);
					if (real_y < 620) { targetq = 8; asd = 2; }
					if (real_y < 2120) speeed = 10000;
					if (cnt1 == 10 && real_y < 820)
					{
						VelCrl(1, -5000);
						VelCrl(2, 5000);
						delay_ms(500);
						targetq = 8; asd = 2;
						cnt1 = 0;
					}
					if ((PhoElec_RF == 1 || PhoElec_LF == 1 || markstop == 1) && real_y>820)
					{
						flag = 1;
					}
					if (flag == 0)
					{
						x0 = real_x;
						y0 = real_y;
						angle0 = real_angle;

					}
					if (flag == 1)
					{
						VelCrl(1, -5000);
						VelCrl(2, 5000);
						if (distance > 200)
						{
							VelCrl(1, 5000);
							VelCrl(2, 5000);
						}
						if (fabs(angle0 - real_angle) > 70)
						{
							VelCrl(1, 5000);
							VelCrl(2, -5000);
							delay_ms(1000);
							flag = 0;
						}

					}
					break;
				}
				case 8:
				{
					if (asd == 2)asd = 2;
					if (real_angle < 92 && real_angle>88 && asd == 6)
						asd = 3;
					if (((KEY_R == 1 && KEY_L == 1) || cnt == 10) && asd == 3)
					{
						asd = 7;
						if (KEY_R == 1 && KEY_L == 1)
							rad1 = 90 - getAngle();
					}
					switch (asd)
					{
					case 2:
						smallmove(2);
						if (real_angle<2 && real_angle>-10)
							asd = 5;
						x0 = real_x;
						y0 = real_y;
						break;
					case 5:
						VelCrl(1, 7000);
						VelCrl(2, -7000);
						jishu++;
						if (distance > 250 && jishu > 2)
						{
							asd = 6;
							jishu = 0;
						}
						if (jishu == 100 && distance < 100)
						{
							situation = 11;
							permit = 1;
							step = 3;
							cnt = -10;
							kaqiu = 0;
							jishu = 0;
						}
						break;
					case 6:
						smallmove(1);
						break;
					case 3:
						adjust = cl_angle(90, real_angle);
						VelCrl(1, -(10000 - adjust));
						VelCrl(2, (10000 + adjust));
						break;
					case 7:
						VelCrl(1, 5000);
						VelCrl(2, -5000);
						delay_ms(200);
						asd = 4;
						break;
					case 4:
						smallmove(4);
						cnt1 = 0;
						break;
					}
					if (real_angle<2 && real_angle>-2 && asd == 4)
					{
						targetq = 1;
						asd = 2;
					}
					if (cnt1 == 35 && asd == 4)
					{
						VelCrl(1, -5000);
						VelCrl(2, 5000);
						delay_ms(500);
						cnt1 = 0;

						targetq = 1;
						asd = 2;
					}
					if (cnt1 == 35 && asd == 3)
					{
						VelCrl(1, 5000);
						VelCrl(2, -5000);
						delay_ms(500);
						cnt1 = 0;
					}
					if (cnt1 == 35 && asd == 6)
					{
						VelCrl(1, 5000);
						VelCrl(2, -5000);
						delay_ms(500);
						cnt1 = 0;
					}
					if (cnt1 == 10 && asd != 4 && asd != 3 && asd != 6)
					{
						VelCrl(1, -5000);
						VelCrl(2, 5000);
						delay_ms(500);
						cnt1 = 0;
					}
					break;
				}
				}

				if (i >= 2) { statustask = 7; order = 2; i = 0; }

				USART_OUT(UART5, (uint8_t *)"real_angle %d\t", (int)real_angle);
				USART_OUT(UART5, (uint8_t *)"asd %d\t", (int)asd);
				USART_OUT(UART5, (uint8_t *)"targetq %d\t", (int)targetq);
				USART_OUT(UART5, (uint8_t *)"cnt1 %d\t\r\n", (int)cnt1);
				USART_OUT(UART5, (uint8_t *)"jishu %d\r\n", (int)jishu);
				USART_OUT(UART5, (uint8_t *)"markstop %d\t", (int)markstop);
				//				USART_OUT(UART5, (uint8_t *)"targetq %d\t", (int)targetq);
				//				USART_OUT(UART5, (uint8_t *)"i %d\t\r\n", (int)i);

			}

			else if (statustask == 5)            //摄像头找球  hunzibushouqiu09
			{
				color();
				switch (nn)
				{
				case 1:
					num = 21000; bound12 = 2800; bound23 = 2800; bound34 = 2000; bound14 = 2000;
					if ((real_x<bound12&&real_y < bound14) || (real_x < bound12&&real_x>1600 && real_y<2175 && real_y>bound14))//区域一
						exangle = 0;
					if ((real_x > bound12&&real_y < bound23) || (real_x < bound12&&real_x>3200 && real_y < bound23&&real_y>2175))
						exangle = 90;
					if ((real_x > bound34&&real_y>bound23) || (real_x<3200 && real_x>bound34&&real_y < bound23&&real_y>2725))
						exangle = -180;
					if ((real_x<bound34&&real_y>bound14) || (real_x<1600 && real_x>bound34&&real_y<2725 && real_y>bound14))
						exangle = -90;

					adjust = 1.2*cl_angle1(exangle, real_angle);
					VelCrl(1, (num + adjust));
					VelCrl(2, -(num - adjust));
					break;
				case 2:
					num = 21000; bound12 = 3100; bound23 = 3100; bound34 = 1800; bound14 = 1800;
					if ((real_x<bound12&&real_y < bound14) || (real_x < bound12&&real_x>1600 && real_y<2175 && real_y>bound14))//区域一
						exangle = 0;
					if ((real_x > bound12&&real_y < bound23) || (real_x < bound12&&real_x>3200 && real_y < bound23&&real_y>2175))
						exangle = 90;
					if ((real_x > bound34&&real_y>bound23) || (real_x<3200 && real_x>bound34&&real_y < bound23&&real_y>2725))
						exangle = -180;
					if ((real_x<bound34&&real_y>bound14) || (real_x<1600 && real_x>bound34&&real_y<2725 && real_y>bound14))
						exangle = -90;

					adjust = 1.2*cl_angle1(exangle, real_angle);
					VelCrl(1, (num + adjust));
					VelCrl(2, -(num - adjust));
					break;
				}


				USART_OUT(UART5, (uint8_t *)"  jishu=%d", (int)jishu);
				if (jishu >= 3) { statustask = 4; targetq = 1; asd = 2; jishu = 0; nn = 1; }
				//	USART_OUT(UART5, (uint8_t *)"  jishu=%d",(int)jishu);
				USART_OUT(UART5, (uint8_t *)"  di=%d", (int)di);
				USART_OUT(UART5, (uint8_t *)"  nn=%d\r\n", (int)nn);
			}

			else if (statustask == 7)			      //矫正放球条件判
			{
				//color();
				//	freedom(3200,3200,1600,1650,20000);
				num = 17000; bound12 = 3400; bound23 = 3000; bound34 = 1300; bound14 = 1850;
				if ((real_x<bound12&&real_y < bound14) || (real_x < bound12&&real_x>1600 && real_y<2175 && real_y>bound14))//区域一
					exangle = 0;
				if ((real_x > bound12&&real_y < bound23) || (real_x < bound12&&real_x>3200 && real_y < bound23&&real_y>2175))
					exangle = 90;
				if ((real_x > bound34&&real_y>bound23) || (real_x<3200 && real_x>bound34&&real_y < bound23&&real_y>2725))
					exangle = -180;
				if ((real_x<bound34&&real_y>bound14) || (real_x<1600 && real_x>bound34&&real_y<2725 && real_y>bound14))
					exangle = -90;

				adjust = 1.2*cl_angle1(exangle, real_angle);
				VelCrl(1, (num + adjust));
				VelCrl(2, -(num - adjust));
				if (real_x > 850 && real_y < 1700 && real_x < 2100)
				{
					VelCrl(1, 0);
					VelCrl(2, 0);
					delay_ms(500);
					statustask = 8;
					x0 = real_x; y0 = real_y;
				}

			}
			else if (statustask == 8)            //放球前的矫正
			{
				if (cnt1 >= 300)                    /////////
				{
					USART_OUT(UART5, (uint8_t*)"c \t");
					situation = 11;
					permit = 1;
					step = 1;
					cnt1 = -10;
					kaqiu = 0;
				}
				if (count == 0)
				{
					switch (biaoi)
					{
					case 1://靠墙
					{
						if (cnt == 50 && fabs(real_angle) < 30 && real_x < 2400)
						{
							VelCrl(1, 10000);
							VelCrl(2, -10000);
							delay_ms(700);		cnt = 0;
						}
						if (cnt == 50 && fabs(real_angle) > 160 && real_x > 2400)
						{
							VelCrl(1, -10000);
							VelCrl(2, 10000);
							delay_ms(700);	statustask = 7;	cnt = 0;
						}
						if (real_x < 1600)
						{
							if (real_angle < 100 && real_angle>80)
							{
								exangle = 90;
								adjust = cl_angle(exangle, real_angle);
								if (distance < 1000)
								{
									VelCrl(1, -(12000 - adjust));
									VelCrl(2, (12000 + adjust));
								}
								else
								{
									VelCrl(1, -(5000 - adjust));
									VelCrl(2, (5000 + adjust));
								}
								if ((KEY_L == 1 && KEY_R == 1) || cnt == 30)
								{
									cnt = 0;
									if (KEY_L == 1 && KEY_R == 1 && fabs(90 - real_angle) < 30)
									{
										righty = 1;
										rad1 = 90 - getAngle();
										biaoi = 40;
									}
									else
										biaoi = 2;
									if (colorgroundright != 103 || colorgroundleft != 103)
									{
										biaoi = 30;
										if (righty == 1)
										{
											already = 1;
											biaoi = 40;
										}
										x0 = real_x;
										y0 = real_y;
									}
								}
							}
							else
							{
								exangle = 90;
								adjust = cl_angle(exangle, real_angle);
								VelCrl(1, adjust);
								VelCrl(2, adjust);
							}
						}
						else
						{
							exangle = 0;
							adjust = cl_angle(exangle, real_angle);
							VelCrl(1, -(10000 - adjust));
							VelCrl(2, (10000 + adjust));
							if (cnt == 30)
							{
								VelCrl(1, 10000);
								VelCrl(2, -10000);
								delay_ms(700);
								cnt = 0;
							}
						}

						break;
					}
					case 40:
						lastb1 = b1 - 60;
						rightb = lastb1;
						if (already == 0)
							biaoi = 2;
						if (already == 1)
						{
							biaoi = 30;
							already = 0;
						}
						break;
					case 2://转向
					{
						sub = 0;
						if (cnt > 150)
						{
							flag = 1;
							x0 = real_x;
							y0 = real_y;
						}
						if (flag == 1)
						{
							VelCrl(1, 10000);
							VelCrl(2, -10000);
							if (distance > 50)
								flag = 0;
						}
						if (flag == 0)
						{
							VelCrl(1, 0);
							VelCrl(2, -5000);
						}
						if (real_angle > -1 && real_angle < 45)
						{
							biaoi = 3;
							x0 = real_x;
							y0 = real_y;
						}
						break;
					}
					case 30:
						jishu++;
						if (jishu == 1)
						{
							x0 = real_x; y0 = real_y;
						}
						if (real_angle<30 && real_angle>-30)
						{
							flag = 0;
							exangle = 0;
							adjust = cl_angle(exangle, real_angle);
							VelCrl(1, -(7000 - adjust));
							VelCrl(2, +(7000 + adjust));
						}
						else
							flag = 1;
						if (flag == 1)
						{
							exangle = 0;
							adjust = cl_angle(exangle, real_angle);
							VelCrl(1, 1000);
							VelCrl(2, -5000);
						}
						if (distance > 500 && jishu > 2 && flag == 0)
						{
							biaoi = 3;
							jishu = 0;
							x0 = real_x;
							y0 = real_y;
						}
						break;
					case 3://去扫色标
					{
						color();
						if (markoo == 0)
						{
							exangle = 0;
							adjust = cl_angle(exangle, real_angle);
							VelCrl(1, (8000 + adjust));
							VelCrl(2, -(8000 - adjust));
						}
						if ((distance > 2000 || (cnt > 100 && real_x > 2400) || real_x > 4000) && (markoo == 0))      // distance记的是什么距离？
						{
							biaoi = 7;
							jishu4 = 0; jishu = 0;
							cnt = 0;
						}
						if (PhoElec_LF == 1 || PhoElec_RF == 1)
						{
							markoo = 1;
						}
						if (markoo == 1) 		//meifanghu
						{
							if (cnt >= 50)
							{
								VelCrl(1, 8000);
								VelCrl(2, -8000);
								delay_ms(1000);
							}
							if (cnt >= 100)
							{
								VelCrl(1, -8000);
								VelCrl(2, 8000);
								delay_ms(1000);
								cnt = 0;
							}
							jishu++;
							if (jishu < 80)
							{
								exangle = 0;
								adjust = cl_angle(exangle, real_angle);
								VelCrl(1, -(5000 - adjust));
								VelCrl(2, (5000 + adjust));
							}
							else
							{
								exangle = 90;
								adjust = cl_angle(exangle, real_angle);
								VelCrl(1, (10000 + adjust));
								VelCrl(2, -(10000 - adjust));
							}
							if (real_y > 1200) { statustask = 11; jishu = 0; markoo = 0; step_to_warehouse = 1; }

						}
						//	USART_OUT(UART5,(uint8_t*)"sub %d\t",(int)sub);
						if (colorgroundright != 103 && real_x > 2200 && markoo == 0)
						{
							biaoi = 5;
							jishu = 0;
							if (righty == 1)
							{
								count = 1;
								biaoi = 1; righty = 0;
							}
							sub = 2;
						}
						//	USART_OUT(UART5,(uint8_t*)"getcha %d\t",(int)getcha2(lasta1));
						//		USART_OUT(UART5,(uint8_t*)"exangle %d\t",(int)exangle);

						break;
					}
					case 5://靠墙
					{
						if (kaqiu == 0)
						{
							if (mark == 4)
							{
								if (flag == 0)
								{
									x0 = real_x;
									y0 = real_y;
								}
								flag = 1;
								if (flag == 1)
								{
									jishu++;
									exangle = 0;
									adjust = cl_angle(exangle, real_angle);
									VelCrl(1, (5000 + adjust));
									VelCrl(2, -(5000 - adjust));
									if (distance > 600 && jishu > 2)
									{
										jishu = 0;
										kaqiu = 0;
										flag = 0;
										mark = 0;
										VelCrl(1, 0);
										VelCrl(2, -5000);
										delay_ms(500);
									}
								}
							}
							else {
								if (KEY_L == 0 && KEY_R == 0)
								{
									if (real_angle < 100 && real_angle>80)
									{
										exangle = 90;
										adjust = cl_angle(exangle, real_angle);
										VelCrl(1, -(8000 - adjust));
										VelCrl(2, (8000 + adjust));
									}
									else
									{
										exangle = 90;
										adjust = cl_angle(exangle, real_angle);
										VelCrl(1, adjust);
										VelCrl(2, adjust);
									}
								}

								if (KEY_L == 0 && KEY_R == 0 && cnt >= 40)
								{
									kaqiu = 1; cnt = 0;
								}
								if (KEY_L == 1 && KEY_R == 1)
									jishu4++;
								if (KEY_L == 1 && KEY_R == 1 && jishu4 >= 20)
								{
									biaoi = 6;	rad1 = 90 - getAngle(); jishufor = 0; jishu4 = 0;
								}
								if (KEY_L == 1 && KEY_R == 0 && cnt >= 10 && cnt < 100)//右触发
								{
									VelCrl(1, -5000);
									VelCrl(2, 10000);
								}
								if (KEY_L == 0 && KEY_R == 1 && cnt >= 10 && cnt < 100)
								{
									VelCrl(1, -10000);
									VelCrl(2, 5000);
								}
								if (KEY_L == 1 && KEY_R == 0 && cnt >= 100)//右触发
								{
									kaqiu = 1; cnt = 0;
								}
								if (KEY_L == 0 && KEY_R == 1 && cnt >= 100)
								{
									kaqiu = 1; cnt = 0;
								}
							}
						}
						if (kaqiu == 1)
						{

							if (flag == 0)
							{
								x0 = real_x;
								y0 = real_y;
							}
							flag = 1;
							if (flag == 1)
							{
								if (cnt > 70)
								{
									situation = 11;
									permit = 1;
									step = 3;
									cnt = -10;
									kaqiu = 0;
								}
								jishu++;
								exangle = 120;
								adjust = cl_angle(exangle, real_angle);
								VelCrl(1, (5000 + adjust));
								VelCrl(2, -(5000 - adjust));
								if (distance > 500 && jishu > 2)
								{
									jishu = 0;
									kaqiu = 0;
									flag = 0;
									mark++;
								}
							}
						}
						break;
					}
					case 6://矫正
					{
						lastb1 = b1 - 60;
						rightb = lastb1;
						biaoi = 1; count++;
						break;
					}
					case 7:
						VelCrl(1, -8000);
						VelCrl(2, 10000);
						delay_ms(400);
						biaoi = 8;
						break;
					case 8:
					{
						if (kaqiu == 0)
						{
							if (mark == 4)
							{
								if (flag == 0)
								{
									x0 = real_x;
									y0 = real_y;
								}
								flag = 1;
								if (flag == 1)
								{
									jishu++;
									exangle = -90;
									adjust = cl_angle(exangle, real_angle);
									VelCrl(1, (5000 + adjust));
									VelCrl(2, -(5000 - adjust));
									if (distance > 600 && jishu > 2)
									{
										jishu = 0;
										kaqiu = 0;
										flag = 0;
										mark = 0;
										VelCrl(1, 0);
										VelCrl(2, -5000);
										delay_ms(500);
									}
								}
							}
							else {
								if (KEY_L == 0 && KEY_R == 0)
								{
									if (real_angle < 100 && real_angle>80)
									{
										exangle = 90;
										adjust = cl_angle(exangle, real_angle);
										VelCrl(1, -(8000 - adjust));
										VelCrl(2, (8000 + adjust));
									}
									else
									{
										exangle = 90;
										adjust = cl_angle(exangle, real_angle);
										VelCrl(1, adjust);
										VelCrl(2, adjust);
									}
								}

								if (KEY_L == 0 && KEY_R == 0 && cnt >= 70)
								{
									kaqiu = 1; cnt = 0;
								}
								if (KEY_L == 1 && KEY_R == 1)
									jishu4++;
								if (KEY_L == 1 && KEY_R == 1 && jishu4 >= 50 && fabs(real_angle - 90) < 20)
								{
									biaoi = 12;	rad1 = 90 - getAngle(); jishufor = 0; jishu4 = 0; x0 = real_x; y0 = real_y;
								}
								if (KEY_L == 1 && KEY_R == 1 && jishu4 >= 50 && fabs(real_angle - 90) > 20)
								{
									VelCrl(1, 10000);
									VelCrl(2, -10000);
									delay_ms(1000);
								}
								if (KEY_L == 1 && KEY_R == 0 && cnt >= 10 && cnt < 100)//右触发
								{
									VelCrl(1, -5000);
									VelCrl(2, 10000);
								}
								if (KEY_L == 0 && KEY_R == 1 && cnt >= 10 && cnt < 100)
								{
									VelCrl(1, -10000);
									VelCrl(2, 5000);
								}
								if (KEY_L == 1 && KEY_R == 0 && cnt >= 100)//右触发
								{
									kaqiu = 1; cnt = 0;
								}
								if (KEY_L == 0 && KEY_R == 1 && cnt >= 100)
								{
									kaqiu = 1; cnt = 0;
								}
							}
						}
						if (kaqiu == 1)
						{

							if (flag == 0)
							{
								x0 = real_x;
								y0 = real_y;
							}
							flag = 1;
							if (flag == 1)
							{
								if (cnt > 70)
								{
									situation = 11;
									permit = 1;
									step = 3;
									cnt = -10;
									kaqiu = 0;
								}
								jishu++;
								exangle = 120;
								adjust = cl_angle(exangle, real_angle);
								VelCrl(1, (5000 + adjust));
								VelCrl(2, -(5000 - adjust));
								if (distance > 500 && jishu > 2)
								{
									jishu = 0;
									kaqiu = 0;
									flag = 0;
									mark++;
								}
							}
						}
						break;
					}
					case 12:
						lastb1 = b1 - 60;
						rightb = lastb1;
						biaoi = 9;
						x0 = real_x; y0 = real_y;
						break;
					case 9:
						exangle = 90; jishu++;
						adjust = cl_angle(exangle, real_angle);
						VelCrl(1, (10000 + adjust));
						VelCrl(2, -(10000 - adjust));
						if (distance > 600 && jishu > 2)
						{
							biaoi = 10; x0 = real_x; y0 = real_y; jishu = 0;
						}
						break;
					case 10:
					{
						if (kaqiu == 0)
						{
							if (mark == 4)
							{
								if (flag == 0)
								{
									x0 = real_x;
									y0 = real_y;
								}
								flag = 1;
								if (flag == 1)
								{
									jishu++;
									exangle = -90;
									adjust = cl_angle(exangle, real_angle);
									VelCrl(1, (5000 + adjust));
									VelCrl(2, -(5000 - adjust));
									if (distance > 600 && jishu > 2)
									{
										jishu = 0;
										kaqiu = 0;
										flag = 0;
										mark = 0;
										VelCrl(1, 0);
										VelCrl(2, -5000);
										delay_ms(500);
									}
								}
							}
							else {
								if (KEY_L == 0 && KEY_R == 0)
								{
									if (real_angle < -160 || real_angle>160)
									{
										exangle = -180;
										adjust = cl_angle(exangle, real_angle);
										VelCrl(1, -(10000 - adjust));
										VelCrl(2, (10000 + adjust));
									}
									else
									{
										exangle = -180;
										adjust = cl_angle(exangle, real_angle);
										VelCrl(1, adjust);
										VelCrl(2, adjust);
									}
								}

								if (KEY_L == 0 && KEY_R == 0 && cnt >= 70)
								{
									kaqiu = 1; cnt = 0;
								}
								if (KEY_L == 1 && KEY_R == 1)
									jishu4++;
								if (KEY_L == 1 && KEY_R == 1 && jishu4 >= 50)
								{
									biaoi = 13;	if (getAngle() > 0)rad1 = -(getAngle() - 180); else rad1 = -(getAngle() + 180); jishufor = 0; jishu4 = 0; x0 = real_x; y0 = real_y;
								}
								if (KEY_L == 1 && KEY_R == 1 && jishu4 >= 50 && fabs(real_angle - 90) > 20)
								{
									VelCrl(1, 10000);
									VelCrl(2, -10000);
									delay_ms(1000);
								}
								if (KEY_L == 1 && KEY_R == 0 && cnt >= 10 && cnt < 100)//右触发
								{
									VelCrl(1, -5000);
									VelCrl(2, 10000);
								}
								if (KEY_L == 0 && KEY_R == 1 && cnt >= 10 && cnt < 100)
								{
									VelCrl(1, -10000);
									VelCrl(2, 5000);
								}
								if (KEY_L == 1 && KEY_R == 0 && cnt >= 100)//右触发
								{
									kaqiu = 1; cnt = 0;
								}
								if (KEY_L == 0 && KEY_R == 1 && cnt >= 100)
								{
									kaqiu = 1; cnt = 0;
								}
							}
						}
						if (kaqiu == 1)
						{

							if (flag == 0)
							{
								x0 = real_x;
								y0 = real_y;
							}
							flag = 1;
							if (flag == 1)
							{
								if (cnt > 70)
								{
									situation = 11;
									permit = 1;
									step = 3;
									cnt = -10;
									kaqiu = 0;
								}
								jishu++;
								exangle = 150;
								adjust = cl_angle(exangle, real_angle);
								VelCrl(1, (5000 + adjust));
								VelCrl(2, -(5000 - adjust));
								if (distance > 500 && jishu > 2)
								{
									jishu = 0;
									kaqiu = 0;
									flag = 0;
									mark++;
								}
							}
						}
					}
					break;
					case 13:
						lasta1 = a1 - 4740;
						righta = lasta1;
						biaoi = 14;
						break;
					case 14:
						if (real_y < 1900 && real_y>1400)
						{
							if (real_x > 2800)
							{
								exangle = -180;
								adjust = cl_angle(exangle + (real_y - 1600) / 6, real_angle);
								VelCrl(1, (10000 + adjust));
								VelCrl(2, -(10000 - adjust));
							}
							else if (real_x > 2450)
							{
								exangle = -180;
								adjust = cl_angle(exangle + (real_y - 1600) / 6, real_angle);
								VelCrl(1, (7000 + adjust));
								VelCrl(2, -(7000 - adjust));
							}
							else
							{
								VelCrl(1, 0);
								VelCrl(2, 0);
								if (NOSPEED)
								{
									biaoi = 1; count = 0; statustask = 11;	step_to_warehouse = 5; righty = 0;
								}
							}
						}
						else if (real_y > 1900)
						{
							adjust = cl_angle(-135, real_angle);
							VelCrl(1, (7000 + adjust));
							VelCrl(2, -(7000 - adjust));
						}
						if (real_y < 1400)
						{
							if (real_x > 2800)
							{
								exangle = -180;
								adjust = cl_angle(exangle, real_angle);
								VelCrl(1, (10000 + adjust));
								VelCrl(2, -(10000 - adjust));
							}
							else if (real_x > 2300)
							{
								exangle = -180;
								adjust = cl_angle(exangle, real_angle);
								VelCrl(1, (7000 + adjust));
								VelCrl(2, -(7000 - adjust));
							}
							else
							{
								VelCrl(1, 0);
								VelCrl(2, 0);
								if (NOSPEED)
								{
									biaoi = 1; count = 0; statustask = 11;	step_to_warehouse = 5; righty = 0;
								}
							}
						}
						break;
					default: break;
					}
				}
				if (count == 1)
				{
					//if((real_x>2700||real_x<2100)&&(sub=2)) {statustask=7;sub=0;}
					if (real_y < 2200)
					{
						if ((cnt >= 30 && real_angle < 90) || (cnt >= 30 && real_x>2400 && real_angle == 90))
							markii = 1;
						if ((cnt >= 30 && real_angle>90) || (cnt >= 30 && real_x < 2400 && real_angle == 90))
							markii = 2;
						if (markii == 0)
						{
							exangle = 90;
							adjust = 1.2*cl_angle(exangle + (real_x - (mubiao - 50)) / 6, real_angle);
							VelCrl(1, (8000 + adjust));
							VelCrl(2, -(8000 - adjust));
						}
						if (markii == 1)
						{
							exangle = 90;
							adjust = cl_angle(exangle, real_angle);
							VelCrl(1, -(5000 - 0.4*adjust));
							VelCrl(2, (5000 + 0.4*adjust));
							if (real_y < 1500 && real_angle < 100 && real_angle>80)
							{
								markii = 0; mubiao -= 50; jinqushibai++;
							}
						}
						if (markii == 2)
						{
							exangle = 90;
							adjust = cl_angle(exangle, real_angle);
							VelCrl(1, -(5000 - 0.4*adjust));
							VelCrl(2, (5000 + 0.4*adjust));
							if (real_y < 1500 && real_angle < 100 && real_angle>80)
							{
								markii = 0; mubiao += 50; jinqushibai++;
							}
						}
						jishu4++;
						if ((PhoElec_RF == 1 || PhoElec_LF == 1))		//considering
						{
							jishu++;
							exangle = 90;
							adjust = cl_angle(exangle, real_angle);
							VelCrl(1, 0.7*adjust);
							VelCrl(2, 0.7*adjust);
						}
						if (jishu4 == 150 && jishu>100)
						{
							statustask = 20; jishu4 = 0; jishu = 0; jinqushibai = 0; markii = 0; mubiao = 2400;
						}
						if (jishu4 == 150)
						{
							jishu4 = 0; jishu = 0;
						}
						if (jinqushibai == 4)
						{
							statustask = 8;
							mark = 0; jinqushibai = 0;
							count = 2; cnt = 0;
						}
					}

					if (2200 <= real_y&&real_y <= 2800)
					{
						jinqushibai = 0;
						VelCrl(1, 0);
						VelCrl(2, 0);
						jishu++;
						if (jishu >= 200)
						{
							statustask = 8; mark = 0;
							count = 2; cnt = 0; jishu = 0;
						}
						if ((PhoElec_L == 1 || PhoElec_R == 1) && (real_x < 2700 && real_x>2100))
						{
							lasta1 = a1 - 2400;	cnt1 = 0;
							VelCrl(1, 3000);
							VelCrl(2, 0);
							delay_ms(300);
							VelCrl(1, 0);
							VelCrl(2, 0);
							OpenLeftDoor();
							delay_ms(1000);
							CloseLeftDoor();
							VelCrl(1, -3000);
							VelCrl(2, 0);
							delay_ms(500);
							VelCrl(1, 0);
							VelCrl(2, 0);
							OpenRightDoor();
							delay_ms(1000);
							CloseRightDoor();
							VelCrl(1, 3000);
							VelCrl(2, 0);
							delay_ms(200);
							VelCrl(1, 0);
							VelCrl(2, 0);

							count = 0;
							biaoi = 1;
							jishu = 0;
							send_CAN();
							markii = 0;
							flag = 0;
							cnt = 0;
							statustask = 10;
						}
					}
				}

				if (count == 2)
				{
					if (real_y < 2000 && real_y>1000)
					{
						exangle = 0;
						adjust = cl_angle(exangle, real_angle);
						VelCrl(1, -(8000 - adjust));
						VelCrl(2, (8000 + adjust));
					}
					else if (real_y < 1000)
					{
						exangle = 90;
						adjust = cl_angle(exangle, real_angle);
						VelCrl(1, (8000 + adjust));
						VelCrl(2, -(8000 - adjust));
					}
					else if (real_y > 2000)
					{
						exangle = 90;
						adjust = cl_angle(exangle, real_angle);
						VelCrl(1, -(8000 - adjust));
						VelCrl(2, (8000 + adjust));
					}
					if (real_x < 800 || markstop == 1)
					{
						count = 0; biaoi = 1; x0 = real_x; y0 = real_y;
						//		flag=0;
					}
					//		}
				}

				{USART_OUT(UART5, (uint8_t*)"f %d\t", (int)flag);
				USART_OUT(UART5, (uint8_t*)"ka %d\t", (int)kaqiu);
				USART_OUT(UART5, (uint8_t*)"sub %d\t", (int)sub);
				USART_OUT(UART5, (uint8_t*)"m %d\t", (int)mark);
				USART_OUT(UART5, (uint8_t*)"j %d\t", (int)jishu);
				USART_OUT(UART5, (uint8_t*)"j4 %d\t", (int)jishu4);
				USART_OUT(UART5, (uint8_t*)"adjust %d\t", (int)adjust);
				USART_OUT(UART5, (uint8_t*)"ct %d\t\r\n", (int)count);
				USART_OUT(UART5, (uint8_t *)"  KEY_L=%d", (int)KEY_L);
				USART_OUT(UART5, (uint8_t *)"  KEY_R=%d", (int)KEY_R);
				USART_OUT(UART5, (uint8_t *)"  PhoElec_L=%d", (int)PhoElec_L);
				USART_OUT(UART5, (uint8_t *)"  PhoElec_R=%d", (int)PhoElec_R);
				USART_OUT(UART5, (uint8_t *)"  PhoElec_LF=%d", (int)PhoElec_LF);
				USART_OUT(UART5, (uint8_t *)"  PhoElec_RF=%d", (int)PhoElec_RF);
				USART_OUT(UART5, (uint8_t *)"  c=%d", (int)cnt);
				USART_OUT(UART5, (uint8_t *)"  ma=%d", (int)markoo);
				USART_OUT(UART5, (uint8_t *)"  bi=%d", (int)biaoi);
				USART_OUT(UART5, (uint8_t *)"  markii=%d", (int)markii);
				USART_OUT(UART5, (uint8_t *)"  x0=%d", (int)x0);
				USART_OUT(UART5, (uint8_t *)"  y0=%d", (int)y0);		}
			}
			else if (statustask == 10)
			{
				if (fabs(real_angle - 90) < 5)
				{
					exangle = 90;
					adjust = cl_angle(exangle, real_angle);
					VelCrl(1, 10000 + adjust);
					VelCrl(2, -(10000 - adjust));
					if (real_y > 2675)
					{
						if (order == 0)		statustask = 3;
						if (order == 1)  	statustask = 5;
						if (order >= 2)		statustask = 15;
						//					   if(order==2)	{statustask = 5;}
						//							if(num_for_putball==-1)
						//							{
						//								statustask = 3;
						//								num_for_putball=0;
						//							}
						//						if(num_for_putball>=3) statustask=5;
						//						if(num_for_putball>=5) statustask=15;

					}
				}
				else
					flag1 = 1;
				if (flag1 == 1)
				{
					exangle = 90;
					if (adjust > 2000) adjust = 2000;
					if (adjust < -2000) adjust = -2000;
					adjust = cl_angle(exangle, real_angle);
					VelCrl(1, adjust);
					VelCrl(2, adjust);
					if (fabs(real_angle - 90) < 5)
						jishu++;
					else
						jishu = 0;
					if (jishu == 10)
					{
						flag1 = 0; jishu = 0;
					}
				}
				//				if(flag==1)
				//				{
				//					if((real_y<2625)&&(PhoElec_RF==0&&PhoElec_LF==0))
				//					{
				//					exangle = 90;
				//					adjust = cl_angle(exangle, real_angle);
				//							VelCrl(1,20000+ adjust);
				//							VelCrl(2, -(20000-adjust));
				//					}
				//					else
				//					{
				//						if(PhoElec_RF==1||PhoElec_LF==1)
				//						{
				//							exangle = 90;
				//							adjust = cl_angle(exangle, real_angle);
				//							VelCrl(1,20000+ adjust);
				//							VelCrl(2, -(20000-adjust));
				//						}
				//						else
				//						{
				//							VelCrl(1,0);
				//							VelCrl(2,0);
				//						}
				//			    }
				//				}
				if (cnt > 300)
				{
					situation = 11;
					permit = 1;
					step = 3;
					cnt = -10;
					kaqiu = 0;
				}
				USART_OUT(UART5, (uint8_t *)"  fla=%d", (int)flag);
			}
			else if (statustask == 9)            //边缘放球
			{

				if (real_y > 2400)
				{
					statustask = 7;
					cnt = 0; cnt1 = 0; cnt2 = 0;
				}
				switch (step)
				{
				case 0:
					jishu5++;/*去放右侧黑球*/
					adjust = cl_angle(90, real_angle);
					VelCrl(1, -(15000 - adjust));
					VelCrl(2, (15000 + adjust));
					if (real_y < 1300) { step = 1; jishu5 = 0; }
					if (cnt == 50)
					{
						situation = 11;
						permit = 1;
						step = 3;
						cnt = -10;
						kaqiu = 0; jishu5 = 0;
					}
					break;
				case 1:jishu5++;
					if (real_x > 1850)
					{
						VelCrl(1, 8000);
						VelCrl(2, 0);
						if (real_angle > 178 || real_angle < -178)
						{
							step = 2; jishu5 = 0;
						}
					}
					else
					{
						adjust = cl_angle(0, real_angle);
						VelCrl(1, -8000);
						VelCrl(2, 0);
						if (real_angle > -2 && real_angle < 2)
						{
							step = 2; jishu5 = 0;
						}
					}
					break;
				case 2:jishu5++;
					if (real_x > 1850)
					{
						adjust = cl_angle(-180, real_angle);
						VelCrl(1, (12000 + adjust));
						VelCrl(2, -(12000 - adjust));
					}
					else
					{
						adjust = cl_angle(0, real_angle);
						VelCrl(1, (12000 + adjust));
						VelCrl(2, -(12000 - adjust));
					}
					if (real_x < 1900 && real_x>1850)
					{
						step = 3; jishu5 = 0;
					}
					break;
				case 3:jishu5++;
					adjust = cl_angle(-90, real_angle);
					VelCrl(1, -(8000 - adjust));
					VelCrl(2, (8000 + adjust));
					if (real_y > 2400) statustask = 7;
					if (KEY_L == 1 || KEY_R == 1 || (cnt > 50 && real_y > 1700))
					{
						if (KEY_L == 1 && KEY_R == 1)
						{
							rad1 = -90 - getAngle();
						}
						step = 4; jishu5 = 0;
						VelCrl(1, 4000);
						VelCrl(2, -4000);
						delay_ms(200);
					}
					break;
				case 4:jishu5++;
					if (KEY_L == 1 && KEY_R == 1)
					{
						lastb1 = b1 - 2050;
					}
					VelCrl(1, 4000);
					VelCrl(2, 0);
					if (real_y < 1500) step = 3;
					if (real_angle<2 && real_angle>-2)
					{
						step = 40; jishu5 = 0;
					}
					break;
				case 40:
					jishu5++;
					if (real_x < 1650)
					{
						adjust = cl_angle(0, real_angle);
						VelCrl(1, (3000 + adjust));
						VelCrl(2, -(3000 - adjust));
					}
					if (real_x > 1650)
					{
						adjust = cl_angle(0, real_angle);
						VelCrl(1, -(3000 - adjust));
						VelCrl(2, (3000 + adjust));
					}
					if (real_x < 1650 && real_x>1600)
					{
						step = 11; jishu5 = 0;
					}
					break;
				case 11:
					jishu5++;
					flag = 1;
					if (flag == 1)
					{
						jishu++;
						if (jishu < 100)
						{
							adjust = cl_angle(10, real_angle);
							VelCrl(1, (3000 + adjust));
							VelCrl(2, -(3000 - adjust));
						}
						if (jishu > 100)
						{
							adjust = cl_angle(-1, real_angle);
							VelCrl(1, -(3000 - adjust));
							VelCrl(2, (3000 + adjust));
						}
						if (jishu > 200) { jishu = 0;		jishu4++; }
					}
					if (PhoElec_L == 1 && jishu4 >= 0)
					{
						step = 12; flag = 0; jishu = 0; jishu4 = 0;		VelCrl(2, 0);
						VelCrl(1, 0); delay_ms(500); jishu5 = 0;
					}
					break;
				case 12:	jishu5++;
					VelCrl(2, 0);
					VelCrl(1, 5000);
					if (cnt >= 10)
					{
						OpenLeftDoor();
						delay_ms(1000);
						CloseLeftDoor();
						VelCrl(2, 5000);
						VelCrl(1, -5000);
						delay_ms(600);
						send_CAN();
						step = 6;
					}
					break;
				case 6:jishu5++;
					if (fabs(Velnow) == 0)
						cnt++;
					else
						cnt = 0;
					distance = real_y - 1880;
					if (distance > 120)
						distance = 120;
					else if (distance < -120)
						distance = -120;
					adjust = cl_angle(0 - distance / 5.0, real_angle);
					VelCrl(1, (5000 + adjust));
					VelCrl(2, -(5000 - adjust));
					if (real_x > 3150)
						step = 7;
					if (cnt == 30)
					{
						flag = 1; x0 = real_x; y0 = real_y; cnt = 0;
					}
					if (flag == 1)
					{
						if (distance < 200)
						{
							VelCrl(1, -5000);
							VelCrl(2, 5000);
						}
						if (distance >= 200)
						{
							distance = 0;
							flag = 0;
						}
					}
					break;
				case 7:jishu5++;
					VelCrl(2, 0);
					VelCrl(1, 0);
					delay_ms(200);
					step = 8;
					break;
				case 8:jishu5++;
					VelCrl(1, -(8000));
					VelCrl(2, 0);
					if (real_angle < -88)
						step = 9;
					break;
				case 9:jishu5++;
					VelCrl(1, 0);
					VelCrl(2, -(8000));
					if ((real_angle < -175) || (markstop == 2))
						step = 10;
					break;
				case 10:jishu5++;
					if (real_x < 3090)
					{
						adjust = cl_angle(-180, real_angle);

						VelCrl(1, -(3000 - adjust));
						VelCrl(2, (3000 + adjust));
					}
					if (real_x > 3130)
					{
						adjust = cl_angle(-180, real_angle);
						VelCrl(1, (3000 + adjust));
						VelCrl(2, -(3000 - adjust));
					}
					if (real_x < 3130 && real_x>3090)
						step = 17;
					break;
				case 17:jishu5++;
					VelCrl(2, 0);
					VelCrl(1, 0);
					flag = 1;
					if (flag == 1)
					{
						jishu++;
						if (jishu < 100)
						{
							adjust = cl_angle(170, real_angle);
							VelCrl(1, (3000 + adjust));
							VelCrl(2, -(3000 - adjust));
						}
						if (jishu > 100)
						{
							adjust = cl_angle(-180, real_angle);
							VelCrl(1, -(3000 - adjust));
							VelCrl(2, (3000 + adjust));
						}
						if (jishu > 200) { jishu = 0;			jishu4++; }
					}
					if (PhoElec_R == 1 && jishu4 >= 0)
					{
						step = 19; flag = 0; jishu = 0; jishu4 = 0;
					}
					break;
				case 19:
					VelCrl(2, 0);
					VelCrl(1, 0);
					if (cnt > 10)
					{
						VelCrl(2, -5000);
						VelCrl(1, 0);
						delay_ms(500);
						OpenRightDoor();
						delay_ms(1000);     /*此处有延时*/
						CloseRightDoor();

						step = 18;
						count = 0;
						jishu = 0;
						VelCrl(2, 5000);
						VelCrl(1, -5000);
						delay_ms(600);
					}
					break;
				case 18:
					switch (jishu)
					{
					case 0:
						adjust = cl_angle(-135, real_angle);
						VelCrl(1, (8000 + adjust));
						VelCrl(2, -(8000 - adjust));
						if (real_y < 1800) jishu++;
						break;
					case 1:
						adjust = cl_angle(0, real_angle);
						VelCrl(1, (5000 + adjust));
						VelCrl(2, -(5000 - adjust));
						if (real_angle<4 && real_angle>-4)
							jishu++;
						break;
					case 2:
						if (order == 0)		statustask = 3;
						if (order == 1)  	statustask = 5;
						if (order >= 2)		statustask = 7;
						jishu = 0;
						break;
					}
					break;


				}
				USART_OUT(UART5, (uint8_t*)"bian %d\t", (int)step);
				USART_OUT(UART5, (uint8_t*)"jishu %d\t", (int)jishu);
				USART_OUT(UART5, (uint8_t*)"PhoElec_R %d\t", (int)PhoElec_R);
				USART_OUT(UART5, (uint8_t*)"PhoElec_L %d\r\n", (int)PhoElec_L);
			}
			else if (statustask == 20)								//边缘放球二号
			{
				switch (step)
				{
				case 1:
				{
					back_anglemove(90, 15000, 180);
					if (real_y < 1300)  step = 2;
					break;
				}
				case 2:
				{
					if (real_angle < -160 || real_angle>160)
						back_anglemove(-180, 10000, 180);
					else
						forward_anglemove(-180, 0, 180);
				if (real_x > 4000) step = 3;
				break;
				}
				case 4:
				{
					forward_anglemove(90, 8000, 180);
					if (real_y > 2400) step = 5;
					break;
				}
				case 5:
				{
					back_anglemove(0, 8000, 180);
					if (KEY_L == 1 || KEY_R == 1 || (cnt > 30 && real_x < 3700))
					{
						step = 6;
						smallmove(5);
						delay_ms(200);				/**短暂地前进，避免卡住墙（此处应有闭环和反馈）**/
					}
					if (real_x < 3700 && cnt>20 && fabs(real_angle - 0) > 10)
					{ 
						statustask = 7; 
						step = 0; 
					}
					if (real_x < 3000) 
					{ 
					statustask = 7; 
					step = 0; 
					}
					break;
				}
				case 6:
				{
					smallmove(6);
					if (real_angle<-88 && real_angle>-92)
						step = 7;
					break;
				}
				case 7:
				{
					if (real_y < 2550)
						back_anglemove(-90, 3000, 180);
					if (real_y > 2600)
						forward_anglemove(-90, 3000, 180);
					if (real_y < 2600 && real_y>2550)
						step = 8;
					break;
				}
				case 8:
				{
					flag = 1;
					if (flag == 1)
					{
						jishu++;
						if (jishu < 100)
							forward_anglemove(-100, 3000, 180);
						if (jishu > 100)
							back_anglemove(-90, 3000, 180);
						if (jishu > 200)
						{
							jishu = 0;		jishu4++;
						}
					}
					if (PhoElec_R == 0 && jishu4 == 6)
					{
						step = 0; statustask = 7; jishu4 = 0;
					}
					if (PhoElec_R == 1 && jishu4 >= 0)
					{
						step = 9; flag = 0; jishu = 0;
						smallmove(7); delay_ms(500);
					}
					break;
				}
				case 9:
				{
					smallmove(8);
					if (cnt >= 10)
					{
						OpenRightDoor();
						delay_ms(1000);
						CloseRightDoor();
						smallmove(9);
						delay_ms(600);
						send_CAN();
						step = 10;
					}
					break;
				}
				case 10:
				{
					forward_anglemove(-90, 8000, 180);
					if (real_y < 1600)
						step = 11;
					break;
				}
				case 11:
				{
					if (real_x > 1600)
						forward_anglemove(-180, 10000, 180);
					else
						forward_anglemove(90, 10000, 180);
					if (real_y > 2300)
						step = 12;
					break;
				}
				case 12:
				{
					smallmove(7);
					if (cnt > 10)step = 13;
					break;
				}
				case 13:
				{
					exangle = -180;
					adjust = cl_angle(exangle + (real_y - 2400) / 6, real_angle);
					VelCrl(1, -(8000 - adjust));
					VelCrl(2, (8000 + adjust));
					if (KEY_L == 1 || KEY_R == 1 || (cnt > 30 && real_x > 1300))
					{
						step = 14; jishu5 = 0;
						smallmove(5);
						delay_ms(200);
					}
					if (real_x > 1300 && cnt > 20 && (real_angle<170 || real_angle>-170)) { statustask = 7; step = 0; }
					if (real_x > 1800) { statustask = 7; step = 0; }
					break;
				}
				case 14:
				{
					smallmove(10);
					if (real_angle<-88 && real_angle>-92)
						step = 15;
					break;
				}
				case 15:
				{
					if (real_y < 2550)
						back_anglemove(-90, 3000, 180);
					if (real_y > 2600)
						forward_anglemove(-90, 3000, 180);
					if (real_y < 2600 && real_y>2550)
						step = 16;
					break;
				}
				case 16:
				{
					flag = 1;
					if (flag == 1)
					{
						jishu++;
						if (jishu < 100)
							forward_anglemove(-80, 3000, 180);
						if (jishu > 100)
							back_anglemove(-90, 3000, 180);
						if (jishu > 200) { jishu = 0;		jishu4++; }
					}
					if (PhoElec_R == 0 && jishu4 == 6)
					{
						step = 0; statustask = 7; jishu4 = 0;
					}
					if (PhoElec_L == 1 && jishu4 >= 0)
					{
						step = 17; flag = 0; jishu = 0;		VelCrl(2, 0);
						VelCrl(1, 0); delay_ms(500);
					}
					break;
				}
				case 17:
				{
					smallmove(10);
					if (cnt >= 10)
					{
						OpenLeftDoor();
						delay_ms(1000);
						CloseLeftDoor();
						smallmove(9);
						send_CAN();
						step = 18;
					}
					jishu = 0;
					break;
				}
				case 18:
				{
					switch (smallstep)
					{
					case 0:
						forward_anglemove(-90, 8000, 180);
						if (real_y < 1800) smallstep++;
						break;
					case 1:
						forward_anglemove(0, 5000, 180);
						if (real_angle<4 && real_angle>-4)
							smallstep++;
						break;
					case 2:
						if (order == 0)		statustask = 3;
						if (order == 1)  	statustask = 5;
						if (order >= 2)		statustask = 7;
						smallstep = 0;
						break;
					}
					break;
				}
				}
				/**串口发数区**/
				{
				USART_OUT(UART5, (uint8_t*)"bian %d\t", (int)step);
				USART_OUT(UART5, (uint8_t*)"cnt %d\t", (int)cnt);
				}

			}
			else if (statustask == 6)            //碰撞超过两次后矫正
			{
				switch (biaoi)
				{
				case 1:
					if (kaqiu == 0)
					{
						if (mark == 3)
						{
							if (flag == 0)
							{
								x0 = real_x;
								y0 = real_y;
							}
							flag = 1;
							if (flag == 1)
							{
								jishu++;
								exangle = whatsexangle();
								adjust = cl_angle(exangle, real_angle);
								VelCrl(1, (5000 + adjust));
								VelCrl(2, -(5000 - adjust));
								if (distance > 900 && jishu > 2)
								{
									jishu = 0;
									kaqiu = 0;
									flag = 0;
									mark = 0;
									VelCrl(1, 0);
									VelCrl(2, -5000);
									delay_ms(500);
								}
							}
						}
						else {
							if (KEY_L == 0 && KEY_R == 0)
							{
								exangle = whatsexangle() + 90;
								adjust = cl_angle(exangle, real_angle);
								VelCrl(1, -(10000 - adjust));
								VelCrl(2, (10000 + adjust));
							}

							if (KEY_L == 0 && KEY_R == 0 && cnt >= 30)
							{
								kaqiu = 1; cnt = 0;
							}
							if (KEY_L == 1 && KEY_R == 1)
								jishu4++;
							if (KEY_L == 1 && KEY_R == 1 && jishu4 >= 50)
							{
								biaoi = 2;	exangle = whatsexangle2();	switch ((int)exangle)
								{
								case 90:rad1 = -(getAngle() - 90); break;
								case -90:rad1 = -(getAngle() + 90); break;
								case 0: rad1 = -(getAngle() - 0); break;
								case -180:if (getAngle() > 0)rad1 = -(getAngle() - 180); else rad1 = -(getAngle() + 180); break;
								}  jishufor = 0; jishu4 = 0;
							}
							if (KEY_L == 1 && KEY_R == 0 && cnt >= 10 && cnt < 50)//右触发
							{
								VelCrl(1, -5000);
								VelCrl(2, 10000);
							}
							if (KEY_L == 0 && KEY_R == 1 && cnt >= 10 && cnt < 50)
							{
								VelCrl(1, -10000);
								VelCrl(2, 5000);
							}
							if (KEY_L == 1 && KEY_R == 0 && cnt >= 50)//右触发
							{
								kaqiu = 1; cnt = 0;
							}
							if (KEY_L == 0 && KEY_R == 1 && cnt >= 50)
							{
								kaqiu = 1; cnt = 0;
							}
						}
					}
					if (kaqiu == 1)
					{

						if (flag == 0)
						{
							x0 = real_x;
							y0 = real_y;
						}
						flag = 1;
						if (flag == 1)
						{
							if (cnt > 70)
							{
								situation = 11;
								permit = 1;
								step = 3;
								cnt = -10;
								kaqiu = 0;
							}
							jishu++;
							exangle = whatsexangle() + 120;
							adjust = cl_angle(exangle, real_angle);
							VelCrl(1, (5000 + adjust));
							VelCrl(2, -(5000 - adjust));
							if (distance > 600 && jishu > 2)
							{
								jishu = 0;
								kaqiu = 0;
								flag = 0;
								mark++;
							}
						}
					}
					break;
				case 2:
					exangle = whatsexangle2();
					switch ((int)exangle)
					{
					case 90:lastb1 = b1 - 60; rightb = lastb1; break;
					case -90:lastb1 = b1 - 4740; rightb = lastb1; break;
					case 0: lasta1 = a1 - 60; righta = lasta1; break;
					case 180: lasta1 = a1 - 4740; righta = lasta1; break;
					}
					//					if(fail!=9)
					//					{
					statustask = statustask_last;
					rightb = lastb1;
					crash = 0; biaoi = 1;
					//					}
					//					else
					//						biaoi=3;
					break;
				}
				USART_OUT(UART5, (uint8_t*)"f %d\t", (int)flag);
				USART_OUT(UART5, (uint8_t*)"m %d\t", (int)mark);
				USART_OUT(UART5, (uint8_t*)"j %d\t", (int)jishu);
				USART_OUT(UART5, (uint8_t*)"k %d\t", (int)kaqiu);
				USART_OUT(UART5, (uint8_t*)"c %d\t\r\n", (int)count);
				USART_OUT(UART5, (uint8_t *)"  x=%d", (int)real_x);
				USART_OUT(UART5, (uint8_t *)"  y=%d", (int)real_y);
				USART_OUT(UART5, (uint8_t *)"  KEY_R=%d", (int)KEY_R);
				USART_OUT(UART5, (uint8_t *)"  KEY_L=%d", (int)KEY_L);
				USART_OUT(UART5, (uint8_t *)"  c=%d", (int)cnt);
				USART_OUT(UART5, (uint8_t *)"  ma=%d", (int)markoo);
				USART_OUT(UART5, (uint8_t *)"  re=%d", (int)real_angle);
				USART_OUT(UART5, (uint8_t *)"  bi=%d", (int)biaoi);
				USART_OUT(UART5, (uint8_t *)"  ji4=%d", (int)jishu4);
				USART_OUT(UART5, (uint8_t *)"  fail=%d", (int)fail);
			}
			else if (statustask == 11)						//色标有车//ka	iubug
			{
				switch (step_to_warehouse)
				{

				case 1:
					if (kaqiu == 0)
					{
						if (mark == 4)
						{
							if (flag == 0)
							{
								x0 = real_x;
								y0 = real_y;
							}
							flag = 1;
							if (flag == 1)
							{
								jishu++;
								exangle = -90;
								adjust = cl_angle(exangle, real_angle);
								VelCrl(1, (5000 + adjust));
								VelCrl(2, -(5000 - adjust));
								if (distance > 600 && jishu > 2)
								{
									jishu = 0;
									kaqiu = 0;
									flag = 0;
									mark = 0;
									VelCrl(1, 0);
									VelCrl(2, -5000);
									delay_ms(500);
								}
							}
						}
						else {
							if (KEY_L == 0 && KEY_R == 0)
							{
								if (real_angle<10 && real_angle>-10)
								{
									exangle = 0;
									adjust = cl_angle(exangle, real_angle);
									VelCrl(1, -(15000 - adjust));
									VelCrl(2, (15000 + adjust));
								}
								else
								{
									cnt1 = 0;
									exangle = 0;
									adjust = cl_angle(exangle, real_angle);
									VelCrl(1, adjust);
									VelCrl(2, adjust);
								}
							}

							if (KEY_L == 0 && KEY_R == 0 && cnt1 >= 10 && (real_angle<10 && real_angle>-10))
							{
								kaqiu = 1; cnt1 = 0;
							}
							if (KEY_L == 1 && KEY_R == 1)
								jishu4++;
							if (KEY_L == 1 && KEY_R == 1 && jishu4 >= 10 && fabs(real_angle) < 20)
							{
								step_to_warehouse = 2;	rad1 = 0 - getAngle(); jishufor = 0; jishu4 = 0; x0 = real_x; y0 = real_y;
							}
							if (KEY_L == 1 && KEY_R == 1 && jishu4 >= 10 && fabs(real_angle) > 20)
							{
								VelCrl(1, 10000);
								VelCrl(2, -10000);
								delay_ms(500);
							}
							if (KEY_L == 1 && KEY_R == 0 && cnt1 >= 10 && cnt1 < 50)//右触发
							{
								VelCrl(1, -5000);
								VelCrl(2, 10000);
							}
							if (KEY_L == 0 && KEY_R == 1 && cnt1 >= 10 && cnt1 < 50)
							{
								VelCrl(1, -10000);
								VelCrl(2, 5000);
							}
							if (KEY_L == 1 && KEY_R == 0 && cnt1 >= 50)//右触发
							{
								kaqiu = 1; cnt1 = 0;
							}
							if (KEY_L == 0 && KEY_R == 1 && cnt1 >= 50)
							{
								kaqiu = 1; cnt1 = 0;
							}
						}
					}
					if (kaqiu == 1)
					{

						if (flag == 0)
						{
							x0 = real_x;
							y0 = real_y;
						}
						flag = 1;
						if (flag == 1)
						{
							if (cnt1 > 50)
							{
								situation = 11;
								permit = 1;
								step = 3;
								cnt1 = -10;
								kaqiu = 0;
							}
							jishu++;
							exangle = -90;
							adjust = cl_angle(exangle, real_angle);
							VelCrl(1, (8000 + adjust));
							VelCrl(2, -(8000 - adjust));
							if (distance > 200 && jishu > 2)
							{
								jishu = 0;
								kaqiu = 0;
								flag = 0;
								mark++;
							}
						}
					}

					break;
				case 2:
					lasta1 = a1 - 60;
					righta = lasta1;
					step_to_warehouse = 3;
					break;
				case 3:
					if (cnt > 150)
					{
						flag = 1;
						x0 = real_x;
						y0 = real_y;
					}
					if (flag == 1)
					{
						VelCrl(1, 20000);
						VelCrl(2, -20000);
						if (distance > 50)
							flag = 0;
					}
					exangle = 0;
					adjust = cl_angle(exangle, real_angle);
					VelCrl(1, (13000 + adjust));
					VelCrl(2, -(13000 - adjust));
					if (real_x > 1600)
						step_to_warehouse = 4;
					break;
				case 4:
					if (real_y < 1900 && real_y>1400)
					{
						if (real_x < 2000)
						{
							exangle = 0;
							adjust = cl_angle(exangle - (real_y - 1600) / 6, real_angle);
							VelCrl(1, (10000 + adjust));
							VelCrl(2, -(10000 - adjust));
						}
						else if (real_x < 2350)
						{
							exangle = 0;
							adjust = cl_angle(exangle - (real_y - 1600) / 6, real_angle);
							VelCrl(1, (7000 + adjust));
							VelCrl(2, -(7000 - adjust));
						}
						else
						{
							VelCrl(1, 0);
							VelCrl(2, 0);
							if (NOSPEED)
							{
								biaoi = 1; count = 0; statustask = 11;	step_to_warehouse = 5; righty = 0;
							}
						}
					}
					else if (real_y > 1900)
					{
						adjust = cl_angle(-45, real_angle);
						VelCrl(1, (7000 + adjust));
						VelCrl(2, -(7000 - adjust));
					}
					if (real_y < 1400)
					{
						if (real_x < 2000)
						{
							exangle = 0;
							adjust = cl_angle(exangle - (real_y - 1600) / 6, real_angle);
							VelCrl(1, (10000 + adjust));
							VelCrl(2, -(10000 - adjust));
						}
						else if (real_x < 2350)
						{
							exangle = 0;
							adjust = cl_angle(exangle - (real_y - 1600) / 6, real_angle);
							VelCrl(1, (7000 + adjust));
							VelCrl(2, -(7000 - adjust));
						}
						else
						{
							VelCrl(1, 0);
							VelCrl(2, 0);
							if (NOSPEED)
							{
								biaoi = 1; count = 0; statustask = 11;	step_to_warehouse = 5; righty = 0;
							}
						}
					}
					break;
				case 5:
					if (real_angle<80 && real_angle>-90)
					{
						VelCrl(1, 3000);
						VelCrl(2, 3000);
					}
					else
					{
						VelCrl(1, -3000);
						VelCrl(2, -3000);
					}
					if (real_angle > 80 && real_angle < 100)
						step_to_warehouse = 6;
					break;
				case 6:
					adjust = cl_angle(90, real_angle);
					VelCrl(1, adjust);
					VelCrl(2, adjust);
					if (fabs(real_angle - 90) < 2)			 jishu++; else jishu = 0;
					if (jishu == 10)
					{
						statustask = 8; count = 1; biaoi = 1; step_to_warehouse = 1; mark = 0; cnt = 0; jishu = 0;
					}
					break;
				default:
					break;

				}
				USART_OUT(UART5, (uint8_t*)"seto %d\t", (int)step_to_warehouse);
				USART_OUT(UART5, (uint8_t*)"real_angle %d\t", (int)real_angle);
			}
			else if (statustask == 12)
			{
				color();
				switch (nn)
				{
				case 1:
					num = 15000; bound12 = 2700; bound23 = 2700; bound34 = 2100; bound14 = 2100;
					if ((real_x<bound12&&real_y < bound14) || (real_x < bound12&&real_x>1600 && real_y<2175 && real_y>bound14))//区域一
						exangle = 0;
					if ((real_x > bound12&&real_y < bound23) || (real_x < bound12&&real_x>3200 && real_y < bound23&&real_y>2175))
						exangle = 90;
					if ((real_x > bound34&&real_y>bound23) || (real_x<3200 && real_x>bound34&&real_y < bound23&&real_y>2725))
						exangle = -180;
					if ((real_x<bound34&&real_y>bound14) || (real_x<1600 && real_x>bound34&&real_y<2725 && real_y>bound14))
						exangle = -90;

					adjust = cl_angle1(exangle, real_angle);
					VelCrl(1, (num + adjust));
					VelCrl(2, -(num - adjust));
					break;
				case 2:
					num = 15000; bound12 = 2600; bound23 = 2600; bound34 = 2200; bound14 = 2200;
					if ((real_x<bound12&&real_y < bound14) || (real_x < bound12&&real_x>1600 && real_y<2175 && real_y>bound14))//区域一
						exangle = 0;
					if ((real_x > bound12&&real_y < bound23) || (real_x < bound12&&real_x>3200 && real_y < bound23&&real_y>2175))
						exangle = 90;
					if ((real_x > bound34&&real_y>bound23) || (real_x<3200 && real_x>bound34&&real_y < bound23&&real_y>2725))
						exangle = -180;
					if ((real_x<bound34&&real_y>bound14) || (real_x<1600 && real_x>bound34&&real_y<2725 && real_y>bound14))
						exangle = -90;

					adjust = cl_angle1(exangle, real_angle);
					VelCrl(1, (num + adjust));
					VelCrl(2, -(num - adjust));
					break;
				}
				if (real_y < 2400 && real_x < 1600 && real_y>2000)
				{
					if (di == 0)
					{
						switch (nn)
						{
						case 1: nn = 2;
							break;
						case 2:  nn = 1;
							break;
						}
						jishu++;
						di = 1;
					}
				}

				if (real_y < 2400 && real_x < 3200 && real_x>3000)	 di = 0;

				USART_OUT(UART5, (uint8_t *)"  jishu=%d", (int)jishu);
				if (jishu >= 3) { statustask = 7; }
				//	USART_OUT(UART5, (uint8_t *)"  jishu=%d",(int)jishu);
				USART_OUT(UART5, (uint8_t *)"  di=%d", (int)di);
				USART_OUT(UART5, (uint8_t *)"  nn=%d\r\n", (int)nn);
			}
			else if (statustask == 13)
			{
				if (real_y < 2200)
				{
					if ((cnt >= 50 && real_angle < 90) || (cnt >= 50 && real_x>2400 && real_angle == 90))
						markii = 1;
					if ((cnt >= 50 && real_angle>90) || (cnt >= 50 && real_x < 2400 && real_angle == 90))
						markii = 2;
					if (markii == 0)
					{
						exangle = 90;
						adjust = 1.3*cl_angle(exangle + (real_x - (mubiao - 30)) / 6, real_angle);
						VelCrl(1, (6000 + adjust));
						VelCrl(2, -(6000 - adjust));
					}
					if (markii == 1)
					{
						exangle = 90;
						adjust = cl_angle(exangle, real_angle);
						VelCrl(1, -(5000 - 0.6*adjust));
						VelCrl(2, (5000 + 0.6*adjust));
						if (real_y < 1500 && real_angle < 100 && real_angle>80)
						{
							markii = 0; mubiao -= 60; jinqushibai++;
						}
					}
					if (markii == 2)
					{
						//	if()
						exangle = 90;
						adjust = cl_angle(exangle, real_angle);
						VelCrl(1, -(5000 - 0.6*adjust));
						VelCrl(2, (5000 + 0.6*adjust));
						if (real_y < 1500 && real_angle < 100 && real_angle>80)
						{
							markii = 0; mubiao += 60; jinqushibai++;
						}
					}
					if ((PhoElec_RF == 1 && real_y>1500) || (PhoElec_LF == 1 && real_y>1500) || markstop == 2)
					{
						if (real_y < 2100 && real_y>1700 && cnt >= 30)  statustask = 9;
					}
					if (jinqushibai == 4)
					{
						statustask = 8; mark = 0; jinqushibai = 0;
						count = 2; cnt = 0;
					}
				}

				if (2200 <= real_y&&real_y <= 2800)
				{
					jinqushibai = 0;
					VelCrl(1, 0);
					VelCrl(2, 0);
					jishu++;
					if (jishu >= 200)
					{
						statustask = 8; mark = 0;
						count = 2; cnt = 0; jishu = 0;
					}
					if (PhoElec_L == 1 && PhoElec_R == 1)
					{
						lasta1 = a1 - 2400;
						delay_ms(200);
						VelCrl(1, 3000);
						VelCrl(2, 3000);
						delay_ms(300);
						OpenLeftDoor();
						delay_ms(1000);
						CloseLeftDoor();
						VelCrl(1, -3000);
						VelCrl(2, -3000);
						delay_ms(500);
						OpenRightDoor();
						delay_ms(1000);
						CloseRightDoor();
						delay_ms(200);

						count = 0;
						biaoi = 1;
						send_CAN();
						markii = 0;
						flag = 0;
						cnt = 0;
						statustask = 10;
						ii_ = 1;
					}
				}
			}
			else if (statustask == 14)
			{
				if (crash != 0) { statustask = 2; cleari(); }
				switch (nn)
				{
				case 1:
					go_small2(1390, 1915, 3410, 2705, 800, 800, 20000);
					if (geti() >= 4) { nn = 2; cleari(); }
					break;
				case 2:
					go_small2(1390, 1915, 3410, 2700, 800, 800, 25000);
					if (geti() >= 4 && real_x < 2000) { statustask = 8; cleari(); }
					break;
				}
			}
			else if (statustask == 15)
			{
				switch (defend)
				{
				case 1:
					//		adjust = cl_angle(exangle, real_angle);
					VelCrl(1, 0);
					VelCrl(2, 0);
					if (cnt >= 10)
					{
						defend = 2;
						cnt = 0;
					}
					break;
				case 2:
					VelCrl(1, 0);
					VelCrl(2, -3000);
					if (real_angle<2 && real_angle>-10)
						defend = 3;
					break;
				case 3:
					if (real_x < 2400)
					{
						adjust = cl_angle(0, real_angle);
						VelCrl(1, (3000 + adjust));
						VelCrl(2, -(3000 - adjust));
					}
					if (real_x > 2500)
					{
						adjust = cl_angle(0, real_angle);
						VelCrl(1, -(3000 - adjust));
						VelCrl(2, (3000 + adjust));
					}
					if (real_x < 2500 && real_x>2400)
						defend = 4;
					break;
				case 4:
					if (PhoElec_RF == 1 || PhoElec_LF == 1)
					{
						exangle = 0;
						adjust = cl_angle(exangle, real_angle);
						VelCrl(1, 20000 + adjust);
						VelCrl(2, -(20000 - adjust));
					}
					else
					{
						VelCrl(1, 0);
						VelCrl(2, 0);
						if (PhoElec_RF == 0 && PhoElec_LF == 0)
							defend = 3;
					}
					break;
				}
				USART_OUT(UART5, (uint8_t*)"defend %d\t", (int)defend);
				USART_OUT(UART5, (uint8_t*)"PhoElec_RF %d\t", (int)PhoElec_RF);
				USART_OUT(UART5, (uint8_t*)"PhoElec_LF %d\r\n", (int)PhoElec_LF);

			}
		}break;

		{
		case 1:
		{
			jishuend++; admit = 0;
			switch (step)
			{
			case 1:

				speed1 = -8000;
				speed2 = -8000;
				//	exangle = whatsexangle()-30;
				adjust = 0;
				jishu++;
				if (jishu == 100 && distance < 50)
				{
					step = 3;
					jishu = 0;
				}
				if (distance > 200)//&&error<60)
				{
					step = 2; jishu = 0;
				}
				break;
			case 2:
				speed1 = 5000;
				speed2 = 5000;
				exangle = whatsexangle();
				adjust = 1.2*cl_angle(exangle, real_angle);
				if (adjust > 5000)adjust = 5000;
				if (adjust < -5000)adjust = -5000;
				if (error < 10)
					jishu++;
				else
					jishu = 0;
				if (jishu >= 1)
				{
					situation = 0; permit = 0; VelCrl(1, 8000); VelCrl(2, -8000); delay_ms(500); crash++; cnt = -50;
					jishu = 0;
					admit = 1;
				}
				break;
			case 3:
				speed1 = -20000;
				speed2 = -20000;
				adjust = 0;
				if (distance > 200)
				{
					step = 2; jishu = 0;
				}
				break;
			}

			VelCrl(1, (speed1 + adjust));
			VelCrl(2, -(speed2 - adjust));
		}
		break;
		case 2:
		{
			jishuend++; admit = 0;
			switch (step)
			{
			case 1:
				speed1 = 20000;
				speed2 = 20000;

				adjust = 0;
				if (distance > 100)
				{
					step = 2; jishu = 0;
				}
				break;
			case 2:
				speed1 = 0;
				speed2 = 0;

				exangle = whatsexangle();
				adjust = 1.2*cl_angle(exangle, real_angle);
				if (adjust > 5000)adjust = 5000;
				if (adjust < -5000)adjust = -5000;
				if (error < 10)
					jishu++;
				else
					jishu = 0;
				if (jishu >= 5)
				{
					situation = 0; permit = 0; VelCrl(1, 8000); VelCrl(2, -8000); delay_ms(500); crash++; cnt = -50;
					admit = 1;
				}
				break;
			}

			VelCrl(1, (speed1 + adjust));
			VelCrl(2, -(speed2 - adjust));
		}break;

		case 3:
		{
			jishuend++; admit = 0;
			speed2 = 0;

			exangle = whatsexangle();
			adjust = (exangle - real_angle) * 200;
			if ((exangle - real_angle) < -180)
				adjust = 7000;
			if (adjust > 8000)	adjust = 8000;
			if (adjust < -5000) adjust = -5000;
			if (error < 10)
				jishu++;
			else
				jishu = 0;
			if (jishu >= 1)
			{
				situation = 0; permit = 0; VelCrl(1, 8000); VelCrl(2, -8000); delay_ms(500); crash++;
				cnt = -50;
				admit = 1;
				jishu = 0;
			}

			VelCrl(1, 0);
			VelCrl(2, -(speed2 - adjust));
		}break;
		case 4:
		{
			situation = 2;
		}
		break;
		case 5:
		{
			jishuend++; admit = 0;

			exangle = whatsexangle();
			adjust = -(exangle - real_angle) * 200;
			if ((exangle - real_angle) < -180)
				adjust = -7000;
			if (adjust > 8000)	adjust = 8000;
			if (adjust < -8000) adjust = -8000;
			if (error < 10)
				jishu++;
			else
				jishu = 0;
			if (jishu >= 1)
			{
				situation = 0; permit = 0; VelCrl(1, 8000); VelCrl(2, -8000); delay_ms(500); crash++; cnt = -50;
				admit = 1; jishu = 0;
			}

			VelCrl(1, adjust);
			VelCrl(2, 0);
		}break;
		case 6:
		{
			jishuend++; admit = 0;
			exangle = whatsexangle();
			switch (step)
			{
			case 1:
				speed1 = -20000;
				speed2 = -20000;

				adjust = 0;
				if (distance > 200)
				{
					step = 2; jishu = 0;
				}
				break;
			case 2:
				speed1 = 0; speed2 = 0;
				exangle = whatsexangle();

				adjust = 1.2*cl_angle(exangle, real_angle);
				if (adjust > 8000)adjust = 8000;
				if (adjust < -8000)adjust = -8000;
				if (error < 10)
					jishu++;
				else
					jishu = 0;
				if (jishu >= 1)
				{
					situation = 0; permit = 0; VelCrl(1, 8000); VelCrl(2, -8000); delay_ms(500); crash++; cnt = -50;
					admit = 1; jishu = 0;
				}
				break;
			}

			VelCrl(1, (speed1 + adjust));
			VelCrl(2, -(speed2 - adjust));
		}break;
		case 7://中间
		{
			jishuend++; admit = 0;
			switch (step)
			{
			case 1:

				speed1 = -8000;
				speed2 = -8000;
				//	exangle = whatsexangle()-90;
				adjust = 0;
				//			if (adjust > 8000)adjust = 8000;
				//			if (adjust < -8000)adjust = -8000;
				if (distance > 300)
				{
					step = 2; jishu = 0;
				}
				break;
			case 2:
				speed1 = 0; speed2 = 0;

				exangle = whatsexangle();
				adjust = 1.2*cl_angle(exangle, real_angle);
				if (adjust > 8000)adjust = 8000;
				if (adjust < -8000)adjust = -8000;
				if (error < 10)
					jishu++;
				else
					jishu = 0;
				if (jishu >= 1)
				{
					situation = 0; permit = 0; VelCrl(1, 8000); VelCrl(2, -8000); delay_ms(500); crash++; cnt = -50;
					admit = 1;
					jishu = 0;
				}
				break;
			}


			VelCrl(1, (speed1 + adjust));
			VelCrl(2, -(speed2 - adjust));
		}break;
		case 8://外面
		{
			jishuend++; admit = 0;
			switch (step)
			{
			case 1:

				speed1 = -8000;
				speed2 = -8000;
				//	exangle = whatsexangle()-30;
				adjust = 0;
				jishu++;
				if (jishu == 100 && distance < 50)
				{
					step = 3;
					jishu = 0;
				}
				if (distance > 200)
				{
					step = 2; jishu = 0;
				}
				break;
			case 2:
				speed1 = 0;
				speed2 = 0;

				exangle = whatsexangle();
				adjust = 1.4*cl_angle(exangle, real_angle);
				if (adjust > 8000)adjust = 8000;
				if (adjust < -8000)adjust = -8000;
				if (error < 10)
					jishu++;
				else
					jishu = 0;
				if (jishu >= 1)
				{
					jishu = 0;
					situation = 0; permit = 0; VelCrl(1, 8000); VelCrl(2, -8000); delay_ms(500); crash++; cnt = -50;
					admit = 1;
				}
				break;
			case 3:
				speed1 = -20000;
				speed2 = -20000;

				adjust = 0;
				if (distance > 200)
				{
					step = 2; jishu = 0;
				}
				break;
			}

			VelCrl(1, (speed1 + adjust));
			VelCrl(2, -(speed2 - adjust));
		}
		break;
		case 9://框内
		{
			jishuend++; admit = 0;
			switch (step)
			{
			case 1:

				speed1 = -8000;
				speed2 = -8000;
				adjust = 0;
				jishu++;
				if (jishu == 100 && distance < 50)
				{
					step = 3;
					jishu = 0;
				}
				if (distance > 250)//&&error<60)
				{
					step = 2; jishu = 0;
				}
				break;
			case 2:
				speed1 = 1000;
				speed2 = 1000;
				exangle = whatsexangle();
				adjust = 1.2*cl_angle(exangle, real_angle);
				if (adjust > 5000)adjust = 5000;
				if (adjust < -5000)adjust = -5000;
				if (error < 10)
					jishu++;
				else
					jishu = 0;
				if (jishu >= 1)
				{
					situation = 0; permit = 0; VelCrl(1, 8000); VelCrl(2, -8000); delay_ms(500); crash++; cnt = -50;
					jishu = 0;
					admit = 1;
				}
				break;
			case 3:
				num = -20000;

				adjust = 0;
				if (distance > 200)
				{
					step = 2; jishu = 0;
				}
				break;
			}

			VelCrl(1, (speed1 + adjust));
			VelCrl(2, -(speed2 - adjust));
		}
		break;
		case 10:
		{
			jishuend++; admit = 0;
			switch (step)
			{
			case 1:

				speed1 = -8000;
				speed2 = -8000;
				adjust = 0;
				if (distance > 200)
				{
					step = 2; jishu = 0;
				}
				break;
			case 2:
				speed1 = 5000; speed2 = 5000;
				exangle = whatsexangle();
				adjust = cl_angle(exangle, real_angle);
				if (adjust > 8000)adjust = 8000;
				if (adjust < -8000)adjust = -8000;
				if (error < 10)
					jishu++;
				else
					jishu = 0;
				if (jishu >= 1)
				{
					situation = 0; permit = 0; VelCrl(1, 8000); VelCrl(2, -8000); delay_ms(500); crash++; cnt = -50;
					admit = 1;
				}
				break;
			}

			VelCrl(1, (speed1 + adjust));
			VelCrl(2, -(speed2 - adjust));
		}break;
		case 11:
		{
			jishuend++; admit = 0;
			switch (step)
			{
			case 1:

				speed1 = -20000;
				speed2 = -20000;
				adjust = 0;
				jishu++;
				if (jishu == 200 && distance < 50)
				{
					step = 3; x0 = real_x; y0 = real_y; jishu = 0;
				}
				if (jishu == 200)jishu = 0;
				if (distance > 150)
				{
					step = 2; jishu = 0;
				}
				VelCrl(1, (speed1 + adjust));
				VelCrl(2, -(speed2 - adjust));
				break;
			case 2:
				speed1 = 0; speed2 = 0;
				exangle = whatsexangle();
				adjust = 1.2*cl_angle(exangle, real_angle);
				mark1++;
				if (mark1 == 200)
				{
					step = 1; x0 = real_x; y0 = real_y; permit = 0;
				}
				if (mark1 == 200)mark1 = 0;
				if (adjust > 8000)adjust = 8000;
				if (adjust < -8000)adjust = -8000;
				if (error < 10)
					jishu++;
				else
					jishu = 0;
				if (jishu >= 1)
				{
					situation = 0; permit = 0; VelCrl(1, 8000); VelCrl(2, -8000); delay_ms(500); crash++; cnt = -50;
					admit = 1;
					jishu = 0;
					step11 = 1;
					jishuend = 0;
					if (em == 1)
					{
						statustask = 7; em = 0;
					}
				}
				VelCrl(1, (speed1 + adjust));
				VelCrl(2, -(speed2 - adjust));
				break;
			case 3:

				speed1 = 25000;
				speed2 = 25000;
				adjust = 0;
				jishu++;
				if (jishu == 200 && distance < 50)
				{
					step = 4;
					jishu = 0; x0 = real_x; y0 = real_y;
				}
				if (jishu == 200)jishu = 0;
				if (distance > 150)
				{
					step = 2; jishu = 0;
				}
				VelCrl(1, (speed1 + adjust));
				VelCrl(2, -(speed2 - adjust));
				break;
			case 4:

				speed1 = -15000;
				speed2 = -10000;
				adjust = 0;
				jishu++;
				if (jishu == 200 && distance < 50)
				{
					step = 5;
					jishu = 0; x0 = real_x; y0 = real_y;
				}
				if (jishu == 200)jishu = 0;
				if (distance > 150)
				{
					step = 2; jishu = 0;
				}
				VelCrl(1, (speed1 + adjust));
				VelCrl(2, -(speed2 - adjust));
				break;
			case 5:

				speed1 = -10000;
				speed2 = -15000;
				adjust = 0;
				jishu++;
				if (jishu == 200 && distance < 50)
				{
					step = 6; x0 = real_x; y0 = real_y;
					jishu = 0;
				}
				if (jishu == 200)jishu = 0;
				if (distance > 150)
				{
					step = 2; jishu = 0;
				}
				VelCrl(1, (speed1 + adjust));
				VelCrl(2, -(speed2 - adjust));
				break;
			case 6:

				speed1 = 10000;
				speed2 = 15000;
				adjust = 0;
				jishu++;
				if (jishu == 200 && distance < 50)
				{
					step = 7; x0 = real_x; y0 = real_y;
					jishu = 0;
				}
				if (jishu == 200)jishu = 0;
				if (distance > 150)
				{
					step = 2; jishu = 0;
				}
				VelCrl(1, (speed1 + adjust));
				VelCrl(2, -(speed2 - adjust));
				break;
			case 7:

				speed1 = 15000;
				speed2 = 10000;
				adjust = 0;
				jishu++;
				if (jishu == 200 && distance < 50)
				{
					step = 1; x0 = real_x; y0 = real_y;
					jishu = 0;
				}
				if (jishu == 200)jishu = 0;
				if (distance > 150)
				{
					step = 2; jishu = 0;
				}
				VelCrl(1, (speed1 + adjust));
				VelCrl(2, -(speed2 - adjust));
				break;
			}
		}
		break;
		case 12:
		{
			jishu++;
			if (OUTSIDE == 1)
				exangle_add = 60;
			if (INSIDE == 1)
				exangle_add = -60;
			if (INSIDE == 0 && OUTSIDE == 0)
			{
				if (PhoElec_L == 1 && PhoElec_R == 0)
					exangle_add = -60;
				if (PhoElec_L == 0 && PhoElec_R == 1)
					exangle_add = 60;
				if (PhoElec_L == 1 && PhoElec_R == 1)
				{
					if (real_x < 800 || real_x>4000 || real_y < 1000 || real_y>3700)
						exangle_add = 60;
					else exangle_add = -60;
				}
			}
			exangle = whatsexangle() + exangle_add;
			adjust = cl_angle(exangle, real_angle);
			VelCrl(1, (12000 + adjust));
			VelCrl(2, -(12000 - adjust));
			if (INSIDE == 1) area = 1;
			if (OUTSIDE == 1)area = 3;
			if (OUTSIDE == 0 && INSIDE == 0) area = 2;
			if (area != area_last || distance >= 500 || jishu >= 200) { situation = 0; permit = 0; jishu = 0; exangle = whatsexangle(); }
			area_last = area;

		}

		}
		}

		{

			USART_OUT(UART5, (uint8_t *)"  cnt2=%d", (int)cnt2);
			USART_OUT(UART5, (uint8_t *)"  la=%d", (int)lasta1);
			USART_OUT(UART5, (uint8_t *)"  KEY_L=%d", (int)KEY_L);
			USART_OUT(UART5, (uint8_t *)"  KEY_R=%d", (int)KEY_R);
			USART_OUT(UART5, (uint8_t*)"situation %d\t", (int)situation);
			//		USART_OUT(UART5,(uint8_t*)"crash %d\t",(int)crash);
			USART_OUT(UART5, (uint8_t*)"num %d\t", (int)numcircle);
			USART_OUT(UART5, (uint8_t*)"sta %d\t", (int)statustask);
			USART_OUT(UART5, (uint8_t *)"  x=%d", (int)real_x);
			USART_OUT(UART5, (uint8_t *)"  y=%d", (int)real_y);
			USART_OUT(UART5, (uint8_t*)"ang %d\t", (int)real_angle);
			USART_OUT(UART5, (uint8_t *)"  cnt=%d", (int)cnt);
			USART_OUT(UART5, (uint8_t *)"  exa=%d", (int)exangle);
			USART_OUT(UART5, (uint8_t *)"  cnt1=%d", (int)cnt1);
			USART_OUT(UART5, (uint8_t *)"  TIME_s=%d\r\n", (int)TIME_s);

		}
	}
}

//			if (whiteball + blackball >= 15)
//			{
//					statustask = 7;
//			}

# define Kp 300.0f
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
#define Kp1   190.0f
#define Ki1   0.0f
#define Kd1   0.0f
static float cl_angle1(float ex, float act)
{
	static float err_sum = 0;
	static float err_last = 0;

	float err = 0;
	float err_vell = 0;

	err = ex - act;
	if (err > 180) err -= 360;        //保证转角为优弧
	if (err < -180) err += 360;

	err_vell = err - err_last;
	err_last = err;

	err_sum += err;

	return (Kp1*err + Ki1*err_sum + Kd1*err_vell);

}
static int16_t getcha(int16_t colorgroundright)
{
	static int16_t color_last = 0;
	int16_t velcolor;
	velcolor = colorgroundright - color_last;
	color_last = colorgroundright;
	return velcolor;
}
static int16_t getcha2(int16_t colorgroundright)
{
	static int16_t color_last = 0;
	int16_t velcolor;
	velcolor = colorgroundright - color_last;
	color_last = colorgroundright;
	return velcolor;
}
void color(void)
{
	static float i = 1, j = 0, midu = 0, lasta2, lastb2, jishuforjudge, what, quit = 0;
	cha = getcha(colorgroundright);
	if (colorinit == 98)
		cha = cha;
	if (colorinit == 114 || colorinit == 111)
	{
		if (cha == -5)	cha = 11;
		if ((cha == 11 || cha == 8) && (gety() < 1000))	cha = -5;
	}
	if (Velnow < -4)
	{
		cha = 0;
	}
	if (quit == 1)
	{
		cha = 0;
	}
	if (cha == 0)
		sub = 0;
	switch (cha)
	{
	case -5:
		if (real_y < 1000)
		{
			what = 1;
			lasta1 = a1 - (2150 - 98 * cos((getAngle())*0.01745) - 163.5*sin((getAngle())*0.01745));//bule
		}
		break;
	case 8:
		if (real_y > 4000)
		{
			if (real_angle > 0)
			{
				what = 1;
				lasta1 = a1 - (2650 + 98 * cos((-180 + getAngle())*0.01745) + 163.5*sin((-180 + getAngle())*0.01745));
			}
			else
			{
				what = 1;
				lasta1 = a1 - (2650 + 98 * cos((getAngle() + 180)*0.01745) + 163.5*sin((getAngle() + 180)*0.01745));
			}
		}//red
		if (real_x > 4000)
		{
			what = 2;
			lastb1 = b1 - (2250 - 98 * cos((getAngle() - 90)*0.01745) - 163.5*sin((getAngle() - 90)*0.01745));
		}//右orange
		else if (real_x < 1000)
		{//左orange
			what = 2;
			lastb1 = b1 - (2550 + 98 * cos((getAngle() + 90)*0.01745) + 163.5*sin((getAngle() + 90)*0.01745));
		}
		break;
	case 11:
		if (real_y > 4000)
		{
			if (real_angle > 0)
			{
				what = 1;
				lasta1 = a1 - (2650 + 98 * cos((-180 + getAngle())*0.01745) + 163.5*sin((-180 + getAngle())*0.01745));
			}
			else

			{
				what = 1;
				lasta1 = a1 - (2650 + 98 * cos((getAngle() + 180)*0.01745) + 163.5*sin((getAngle() + 180)*0.01745));
			}
		}//red
		if (real_x > 4000)
		{
			what = 2;
			lastb1 = b1 - (2250 - 98 * cos((getAngle() - 90)*0.01745) - 163.5*sin((getAngle() - 90)*0.01745));
		}//右orange
		else if (real_x < 1000)
		{//左orange
			what = 2;
			lastb1 = b1 - (2550 + 98 * cos((getAngle() + 90)*0.01745) + 163.5*sin((getAngle() + 90)*0.01745));
		}
		break;
	}
	if (i == 1)
	{
		if (cha == 11 || cha == -5 || cha == 8)
		{
			j = 1;		i = 0;	quit = 1;
			lasta2 = lasta1;
			lastb2 = lastb1;
			if (what == 1)
				lasta1 = righta;
			if (what == 2)
				lastb1 = rightb;
		}
	}
	if (j == 1)
		jishuforjudge++;
	if (jishuforjudge < 20 && jishuforjudge>1)
	{
		if (colorgroundright == 103)
			midu--;
		else
			midu++;
	}
	if ((jishuforjudge == 5 && midu <= -3) || (jishuforjudge == 10 && midu <= -5) || (jishuforjudge == 3 && midu <= -2) || (jishuforjudge == 2 && midu <= -1))
	{
		i = 1;
		jishuforjudge = 0;
		j = 0;
		midu = 0;
		quit = 0; sub = 0;
	}
	if ((jishuforjudge == 5 && midu >= 4) || (jishuforjudge == 10 && midu >= 7) || (jishuforjudge == 1 && midu == -1) || (jishuforjudge == 2 && midu == -1))
	{
		lasta1 = lasta2;
		lastb1 = lastb2;
		righta = lasta1;
		rightb = lastb1; sub = 1;
	}
	if (jishuforjudge >= 20)
	{
		if (midu < 0)
		{
			i = 1;
			jishuforjudge = 0;
			j = 0;
			midu = 0;
			quit = 0;
			if (what == 1)
				lasta1 = righta;
			if (what == 2)
				lastb1 = rightb; sub = 0;
		}
		else	if (midu>13)
		{
			lasta1 = lasta2; sub = 1;
			lastb1 = lastb2;
			righta = lasta1;
			rightb = lastb1;

		}
	}
	if (jishuforjudge == 250)
	{
		i = 1;
		jishuforjudge = 0;
		j = 0;
		midu = 0;
		quit = 0;
	}
	//		if(colorgroundright==98)
	//	{
	//		GPIO_SetBits(GPIOB, GPIO_Pin_15);
	//	}
	//	else
	////		GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	//	//	USART_OUT(UART5, (uint8_t *)"x %d\t", (int)real_x);
	////		USART_OUT(UART5, (uint8_t *)"y %d\t", (int)real_y);
	//		USART_OUT(UART5, (uint8_t *)"co %d\t", (int)colorgroundright);
	//	USART_OUT(UART5, (uint8_t *)"la %d\t", (int)lasta1);
	////	USART_OUT(UART5, (uint8_t *)"lastb1 %d\t", (int)lastb1);
	//	//	USART_OUT(UART5, (uint8_t *)"midu %d\t\r\n", (int)midu);
	//	//			USART_OUT(UART5, (uint8_t *)"judge %d\t", (int)jishuforjudge);
	//					USART_OUT(UART5, (uint8_t *)"mi %d\r\n", (int)midu);
	//					USART_OUT(UART5, (uint8_t *)"qu %d\t", (int)quit);
	//					USART_OUT(UART5, (uint8_t *)"i %d\t", (int)i);
	//				USART_OUT(UART5, (uint8_t *)"b %d\r\n", (int)rightb);
}
