#include  "collectball.h"
#include "stm32f4xx.h"
#include "math.h"
#include "usart.h"
#include "stdlib.h"


static u16 smoothed_data = 0;
void smooth(u16 raw_data)
{
  static u16 tmp_arr[40] = {0};
	static u8 i = 0;
	u16 sum=0;
	u8 j=0, n=0;

	tmp_arr[i++] = raw_data;
	if(i == 40) i = 0;
	for (j = 0; j<40; j++)
	{
		if(tmp_arr[j]>0)
		{
			sum += tmp_arr[j];
			n++;
		}
	}
		smoothed_data = sum / n;
}
u16 getsminfo(void)
{
	return smoothed_data;
}



static u8 ball_num;

unsigned char count_ball(int16_t vel)                  //vel
{

  static u8 cnt;
  static u16 vel_last;
  static u16 speed_sum;
	
	vel =abs(vel);

	if (cnt == 3)
	{
    cnt = 0;
    
		if (vel - vel_last < 0)
		{
			speed_sum += vel_last - vel;   /*keep speed_sum to be positive*/
		}
		else
		{
			if (speed_sum > 110 && speed_sum < 170)
			{
				ball_num ++;

				speed_sum = 0;
			}
			else if (speed_sum >= 170 && speed_sum < 400)
			{
				ball_num +=2;

        speed_sum = 0;
			}
			else
				speed_sum = 0;
		}
		vel_last = vel;
	}
	cnt ++;
  return ball_num;
}

unsigned char ball_in(int16_t vel)                  //vel
{
  u8 ball_in = 0;
  static u8 cnt;
  static u16 vel_last;
  static u16 speed_sum;
	
	vel=abs(vel);

	if (cnt == 3)
	{
    cnt = 0;
    
		if (vel - vel_last < 0)
		{
			speed_sum += vel_last - vel;   /*keep speed_sum to be positive*/
		}
		else
		{
			if (speed_sum > 75 && speed_sum < 150)
			{
				ball_in =1;
				speed_sum = 0;
			}
			else if (speed_sum >= 150 && speed_sum < 400)
			{
				ball_in =2;

        speed_sum = 0;
			}
			else
				speed_sum = 0;
		}
		vel_last = vel;
	}
	cnt ++;
  return ball_in;
}


