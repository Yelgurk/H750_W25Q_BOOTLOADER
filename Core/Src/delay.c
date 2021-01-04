/*
 * delay.c
 *
 *  Created on: 4 янв. 2021 г.
 *      Author: Xell
 */


#include "delay.h"
#include "core_cm7.h"
/*----------------------------------------------------------------------------*/
#define  DWT_CR      (*(volatile unsigned int *)0xE0001000)
#define  DWT_CYCCNT  (*(volatile unsigned int *)0xE0001004)
#define  DEM_CR      (*(volatile unsigned int *)0xE000EDFC)

#define  DEM_CR_TRCENA     (1 << 24)
#define  DWT_CR_CYCCNTENA  (1 <<  0)
/* Exported variables --------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t us_ticks = 400;
/* Exported functions --------------------------------------------------------*/
void delay_init(void)
{
	us_ticks = (uint32_t)(SystemCoreClock / 1000000.0f + 0.5f);
	//DEM_CR |= (unsigned int)DEM_CR_TRCENA;
	//DWT_CYCCNT = (unsigned int)0u;
	//DWT_CR |= (unsigned int)DWT_CR_CYCCNTENA;
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

//延时最大值 2^32 / 400000000 = 10.73741824s
//微秒级阻塞式延时，使用操作系统时可采用挂起任务调度或禁止中断的方式来保证精度
void delay_us(uint32_t us)
{
    uint32_t enter_ticks;
	uint32_t delay_ticks;
	uint32_t ticks;

	enter_ticks = DWT->CYCCNT; // 刚进入时的计数器值
	ticks = 0;
	delay_ticks = us * us_ticks; // 需要的节拍数

	while (ticks < delay_ticks)
	{
		ticks = DWT->CYCCNT - enter_ticks; // 求减过程中，如果发生第一次32位计数器重新计数，依然可以正确计算
	}
}

//毫秒级延时
void delay_ms(uint32_t ms)
{
	while (ms--)
	{
		delay_us(1000);
	}
}

//秒级延时
void delay_xs(uint32_t xs)
{
	while (xs--)
	{
		delay_ms(1000);
	}
}
