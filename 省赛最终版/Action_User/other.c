#include  "move.h"
#include "stm32f4xx.h"
#include "math.h"
#include "usart.h"
#include "stdlib.h"
#include "elmo.h"
#include "can.h"
#include "stm32f4xx_usart.h"
#include "other.h"
#include "gpio.h"
#include "timer.h"



# define Kp 180.0f
# define Ki 0.0
# define Kd  0.0
extern float real_x,real_y,real_angle;
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


float getvel(float Posx, float Posy, float Angle)
{
	static float x_last = 0, y_last = 0;
	static float velx, vely, vel;
	int8_t fbward;
	velx = Posx - x_last;
	vely = Posy - y_last;
	if (fabs(velx)> 2 || fabs(vely) > 2)
		fbward = (fabs(atan2f(vely, velx) / 3.1415926 * 180 - Angle) < 90) ? 1 : -1;
	else
		fbward = 0;
	vel = fbward * 100 * sqrtf(velx*velx + vely*vely);

	x_last = Posx;
	y_last = Posy;
//				USART_OUT(UART5, (uint8_t *)"velx=%d",(int)velx);
//			USART_OUT(UART5, (uint8_t *)" vely=%d",(int)vely);
//				USART_OUT(UART5, (uint8_t *)" vel=%d\r\n",(int)vel);
	return vel;
}
float getvel1(float Posx, float Posy, float Angle)
{
	static float x_last = 0, y_last = 0;
	static float velx, vely, vel;
	int8_t fbward;
	velx = Posx - x_last;
	vely = Posy - y_last;
	if (fabs(velx)> 0.5 || fabs(vely) > 0.5)
		fbward = (fabs(atan2f(vely, velx) / 3.1415926 * 180 - Angle) < 90) ? 1 : -1;
	else
		fbward = 0;
	vel = fbward * 100 * sqrtf(velx*velx + vely*vely);

	x_last = Posx;
	y_last = Posy;
//				USART_OUT(UART5, (uint8_t *)"velx=%d",(int)velx);
//			USART_OUT(UART5, (uint8_t *)" vely=%d",(int)vely);
//				USART_OUT(UART5, (uint8_t *)" vel=%d\r\n",(int)vel);
	return vel;
}
float getv(void)
{
	float v;
	v = getvel(real_x, real_y, real_angle);
	return v;
}
float getv1(void)
{
	float v;
	v = getvel1(real_x, real_y, real_angle);
	return v;
}


	static float adjust,mark=0;
/*****碰墙后退*****/

/*****碰墙后退*****/


float whatsexangle()
{
	float exangle;
	if (real_x<3410 && real_y<1915) 
		exangle = 0;
	if (real_x>3410 && real_y<2885)		//1.2.3中间，4靠放球区
		exangle = 90;
	if (real_x>1390 && real_y>2885)
		exangle = -180;
	if (real_x<1390 && real_y>1915)
		exangle = -90;
	if (real_x < 3410 && real_x>1390 && real_y > 1915 && real_y < 2885)
	{
		if (real_angle > 0)
			exangle = 90;
		else
			exangle = -90;
	}
	return exangle;
}
float whatsexangle2()
{
	float exangle;
	if (real_angle<135 && real_angle>45) 
		exangle = 90;
	if (real_angle>-45 && real_angle<45)		//1.2.3中间，4靠放球区
		exangle = 0;
	if (real_angle>135 || real_angle<-135)
		exangle = -180;
	if (real_angle<-45 && real_angle>-135)
		exangle = -90;
	return exangle;
}
static int iii=400;

	extern int TIME_s;

/*****碰墙后退*****/
static int getcha1(float colorgroundright)
{
	static int16_t color_last = 0;
	int16_t velcolor;
	velcolor = colorgroundright - color_last;
	color_last = colorgroundright;
	return velcolor;
}


