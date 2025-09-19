#include "stm32f10x.h"

uint32_t state = 0;

void sleep_us(uint32_t n)
{
	SysTick->LOAD = 72 * n;
	SysTick->VAL = 0x00;
	SysTick->CTRL = 0x00000005;
	while (!(SysTick->CTRL & 0x00010000));
	SysTick->CTRL = 0x00000004;
}

void sleep(uint32_t n)
{
	for (uint32_t i = 0; i < 1000; ++i)
		sleep_us(n);
}

void sleep_s(uint32_t n)
{
	for (uint32_t i = 0; i < 1000000; ++i)
		sleep_us(n);
}

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef ga, gb;
	ga.GPIO_Mode = GPIO_Mode_Out_PP;
	ga.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	ga.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &ga);
	
	gb.GPIO_Mode = GPIO_Mode_IPU;
	gb.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10;
	gb.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gb);
	
	while(1)
	{
		if (!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)))
			state = 0;
		if (!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)))
			state = 1;
		if (!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)))
			state = 2;
		switch (state)
		{
			case 0:
			{
				GPIO_SetBits(GPIOA, GPIO_Pin_0);
				GPIO_ResetBits(GPIOA, GPIO_Pin_1);
				GPIO_ResetBits(GPIOA, GPIO_Pin_2);
			} 
			break;
			case 1:
			{
				GPIO_SetBits(GPIOA, GPIO_Pin_1);
				GPIO_ResetBits(GPIOA, GPIO_Pin_0);
				GPIO_ResetBits(GPIOA, GPIO_Pin_2);
			} 
			break;
			case 2:
			{
				GPIO_SetBits(GPIOA, GPIO_Pin_2);
				GPIO_ResetBits(GPIOA, GPIO_Pin_1);
				GPIO_ResetBits(GPIOA, GPIO_Pin_0);
			} 
			break;
			default:break;
		}
			
	}
}
