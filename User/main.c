#include "stm32f10x.h"
#include "OLED.h"
#include <math.h>
#include <stdio.h>


//unsigned long long tick = 0;
volatile uint32_t phi = 0;
uint32_t state = 0;
// 全局变量，存储从程序开始以来的毫秒数
volatile uint32_t tick = 0;

const uint16_t sine_table[256] = {
     250,  262,  275,  288,  301,  315,  329,  343,  357,  372,
     386,  401,  416,  431,  447,  462,  478,  494,  509,  525,
     541,  557,  573,  589,  605,  621,  637,  652,  668,  683,
     699,  714,  729,  743,  758,  772,  786,  800,  813,  826,
     839,  851,  863,  874,  885,  896,  906,  916,  925,  934,
     942,  950,  957,  964,  970,  976,  981,  985,  989,  992,
     995,  997,  999, 1000, 1000, 1000,  999,  997,  995,  992,
     989,  985,  981,  976,  970,  964,  957,  950,  942,  934,
     925,  916,  906,  896,  885,  874,  863,  851,  839,  826,
     813,  800,  786,  772,  758,  743,  729,  714,  699,  683,
     668,  652,  637,  621,  605,  589,  573,  557,  541,  525,
     509,  494,  478,  462,  447,  431,  416,  401,  386,  372,
     357,  343,  329,  315,  301,  288,  275,  262,  250,  238,
     226,  215,  203,  193,  182,  172,  162,  152,  143,  134,
     126,  118,  110,  102,   95,   88,   82,   76,   70,   64,
      59,   54,   49,   45,   41,   37,   33,   30,   27,   24,
      21,   19,   17,   15,   13,   11,   10,    8,    7,    6,
       5,    4,    3,    3,    2,    2,    1,    1,    1,    1,
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    1,    1,    1,    1,    2,
       2,    3,    3,    4,    5,    6,    7,    8,   10,   11,
      13,   15,   17,   19,   21,   24,   27,   30,   33,   37,
      41,   45,   49,   54,   59,   64,   70,   76,   82,   88,
      95,  102,  110,  118,  126,  134,  143,  152,  162,  172,
     182,  193,  203,  215,  226,  238,
};

const uint16_t x3_table[256] = {
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       1,    1,    1,    1,    1,    1,    1,    1,    1,    2,
       2,    2,    2,    2,    3,    3,    3,    3,    4,    4,
       4,    4,    5,    5,    5,    6,    6,    7,    7,    7,
       8,    8,    9,    9,   10,   10,   11,   12,   12,   13,
      14,   14,   15,   16,   16,   17,   18,   19,   20,   20,
      21,   22,   23,   24,   25,   26,   27,   28,   29,   31,
      32,   33,   34,   35,   37,   38,   39,   41,   42,   43,
      45,   46,   48,   50,   51,   53,   54,   56,   58,   60,
      61,   63,   65,   67,   69,   71,   73,   75,   77,   79,
      82,   84,   86,   88,   91,   93,   95,   98,  100,  103,
     106,  108,  111,  114,  116,  119,  122,  125,  128,  131,
     134,  137,  140,  143,  147,  150,  153,  157,  160,  164,
     167,  171,  174,  178,  182,  185,  189,  193,  197,  201,
     205,  209,  213,  218,  222,  226,  231,  235,  240,  244,
     249,  253,  258,  263,  268,  273,  278,  283,  288,  293,
     298,  303,  309,  314,  319,  325,  331,  336,  342,  348,
     353,  359,  365,  371,  377,  384,  390,  396,  402,  409,
     415,  422,  429,  435,  442,  449,  456,  463,  470,  477,
     484,  491,  499,  506,  514,  521,  529,  536,  544,  552,
     560,  568,  576,  584,  592,  601,  609,  618,  626,  635,
     643,  652,  661,  670,  679,  688,  697,  706,  716,  725,
     735,  744,  754,  764,  774,  783,  793,  804,  814,  824,
     834,  845,  855,  866,  877,  887,  898,  909,  920,  931,
     943,  954,  965,  977,  988, 1000,
};

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

uint16_t Get_ADC1_Value(uint8_t channel)
{
    /* 重新配置规则组通道（如果通道需要变化） */
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_55Cycles5);

    /* 软件启动转换 */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    /* 等待转换结束 */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

    /* 返回转换结果（读取DR寄存器会自动清除EOC标志）*/
    return ADC_GetConversionValue(ADC1);
}

void PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* 1. 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 开启GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  // 开启TIM3时钟，注意TIM3是APB1上的

    /* 2. 配置PA6和PA7为复用推挽输出 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  // 同时配置PA6和PA7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 3. 配置定时器时基单元 */
    TIM_TimeBaseStructure.TIM_Period = 999;         // 自动重装值 ARR
    TIM_TimeBaseStructure.TIM_Prescaler = 71;       // 预分频器 PSC
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* 4. 配置TIM3的通道1为PWM模式 (PA6) */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;      // PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 输出极性高
    TIM_OCInitStructure.TIM_Pulse = 0; // 初始的捕获/比较值 CCR，即初始占空比
    TIM_OC1Init(TIM3, &TIM_OCInitStructure); // 初始化通道1 (PA6)
    
    /* 5. 配置TIM3的通道2为PWM模式 (PA7) */
    TIM_OCInitStructure.TIM_Pulse = 0; // 重置占空比为0
    TIM_OC2Init(TIM3, &TIM_OCInitStructure); // 初始化通道2 (PA7)

    /* 6. 使能定时器 */
    TIM_Cmd(TIM3, ENABLE);
    
    /* 7. 使能通道1和通道2的预装载寄存器 */
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    /* 8. 使能ARR的预装载寄存器 */
    TIM_ARRPreloadConfig(TIM3, ENABLE);
}

int main(void)
{
	double f = 1;
	uint16_t pwm_value_mode_0 = 0, adc_value = 0;
	char f_str[16] = "0", pwm_str[16] = "0", v_str[16] = "0";
	//uint16_t voltage_level = 0;
	float voltage = 0;
	
	PWM_Init();
	OLED_Init();
	
    //SystemInit();     // 系统时钟初始化（默认72MHz）

	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef ga, ga0, gb;
	ADC_InitTypeDef adc;
	
    ga0.GPIO_Pin = GPIO_Pin_0;
    ga0.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &ga0);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
    adc.ADC_Mode = ADC_Mode_Independent;       // 独立模式
    adc.ADC_ScanConvMode = DISABLE;            // 单通道，禁用扫描模式
    adc.ADC_ContinuousConvMode = DISABLE;      // 单次转换模式
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
    adc.ADC_DataAlign = ADC_DataAlign_Right;   // 数据右对齐
    adc.ADC_NbrOfChannel = 1;                  // 要转换的通道数量
    ADC_Init(ADC1, &adc);

    /* 配置规则组通道：ADC1, 通道0, 采样顺序1, 采样时间55.5周期 */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);

    /* 启用 ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /* 进行ADC校准（建议上电后校准一次）*/
    ADC_ResetCalibration(ADC1); // 复位校准
    while(ADC_GetResetCalibrationStatus(ADC1)); // 等待复位校准完成
    ADC_StartCalibration(ADC1); // 开始校准
    while(ADC_GetCalibrationStatus(ADC1));      // 等待校准完成
	
	ga.GPIO_Mode = GPIO_Mode_Out_PP;
	ga.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	ga.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &ga);
	
	gb.GPIO_Mode = GPIO_Mode_IPU;
	gb.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10;
	gb.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gb);
	
	while(1)
	{
		SysTick->LOAD = 719999;
		SysTick->VAL = 0x00;
		SysTick->CTRL = 0x00000005;
		
		if (!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)))
			state = 0;
		else if (!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)))
			state = 1;
		else if (!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)))
		{
			state = 2;
		}
		adc_value = Get_ADC1_Value(ADC_Channel_0);
		voltage = (float)adc_value * 3.3 / 4095;
		
		switch (state)
		{
			case 0:
			{
				//voltage = (float)Get_ADC1_Value(ADC_Channel_0) * 3.3 / 4095;   // 将值转换为电压值（假设VDDA=3.3V）
				GPIO_SetBits(GPIOA, GPIO_Pin_1);
				TIM_SetCompare1(TIM3, 0);
					
				pwm_value_mode_0 = x3_table[adc_value >> 4];
				TIM_SetCompare2(TIM3, pwm_value_mode_0);
				GPIO_ResetBits(GPIOA, GPIO_Pin_2);
		
				if (!(tick % 50))
				{
					OLED_Clear();
					sprintf(v_str, "%f", voltage);
					OLED_ShowString(0, 0, v_str, OLED_8X16);
					
					sprintf(pwm_str, "%d", pwm_value_mode_0);
					OLED_ShowString(0, 20, pwm_str, OLED_8X16);
					OLED_Update();
				}
			} 
			break;
			case 1:
			{
				TIM_SetCompare1(TIM3, 1000);
				if (voltage < 1)
				{
					GPIO_ResetBits(GPIOA, GPIO_Pin_1);
					TIM_SetCompare2(TIM3, 1000);
					GPIO_ResetBits(GPIOA, GPIO_Pin_2);
				}
				else if (voltage < 2)
				{
					GPIO_ResetBits(GPIOA, GPIO_Pin_1);
					TIM_SetCompare2(TIM3, 0);
					GPIO_SetBits(GPIOA, GPIO_Pin_2);
				}
				else if (voltage < 3)
				{
					GPIO_SetBits(GPIOA, GPIO_Pin_1);
					TIM_SetCompare2(TIM3, 0);
					GPIO_ResetBits(GPIOA, GPIO_Pin_2);
				}
				else
				{
					GPIO_ResetBits(GPIOA, GPIO_Pin_1);
					TIM_SetCompare2(TIM3, 0);
					GPIO_ResetBits(GPIOA, GPIO_Pin_2);
				}
				
			} 
			break;
			case 2:
			{
				TIM_SetCompare2(TIM3, 1000);
				TIM_SetCompare1(TIM3, sine_table[phi >> 24]);
				GPIO_ResetBits(GPIOA, GPIO_Pin_1);
				GPIO_ResetBits(GPIOA, GPIO_Pin_2);
		
				if (!(tick % 50))
				{
					OLED_Clear();
					f = 1 + 4 * voltage;
					f = f < 10 ? f : 10;
					sprintf(v_str, "%f", voltage);
					OLED_ShowString(0, 0, v_str, OLED_8X16);
					
					sprintf(f_str, "%f", f);
					OLED_ShowString(0, 20, f_str, OLED_8X16);
					OLED_Update();
					;
				}
			} 
			break;
			default:break;
		}
		phi += (uint32_t)(42949672.96 * f + 0.5);
		++tick;
		
		while (!(SysTick->CTRL & 0x00010000));
		SysTick->CTRL = 0x00000004;
	}
}
