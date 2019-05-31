#include <includes.h>

void SysTick_Handler(void)
{
	OS_CPU_SR cpu_sr;
	
	OS_ENTER_CRITICAL(); // Tell uC/OS-II that we are starting an ISR
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	
	OSTimeTick(); // Call uC/OS-II's OSTimeTick()
	OSIntExit(); // Tell uC/OS-II that we are leaving the ISR
}

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_DeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |
																GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Delay_ms(int times)
{
	OSTimeDly(OS_TICKS_PER_SEC/1000 * times);
}

void digit_select(int index)
{
	BitAction v[4];
	int i;
	for (i = 0; i < 4; i++)
	{
		v[i] = Bit_RESET;
	}
	v[3-index] = Bit_SET;
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_11, v[0]);
	GPIO_WriteBit(GPIOA, GPIO_Pin_12, v[1]);
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, v[2]);
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, v[3]);
}

void digit_show(int digit, int point)
{
	int segment;
	BitAction v[8];
	int i;
	switch (digit)
	{
		case 0: segment = 0x11; break; // 0b00010001
		case 1: segment = 0xb7; break; // 0b10110111
		case 2: segment = 0x45; break; // 0b01000101
		case 3: segment = 0x25; break; // 0b00100101
		case 4: segment = 0xa3; break; // 0b10100011
		case 5: segment = 0x29; break; // 0b00101001
		case 6: segment = 0x09; break; // 0b00001001
		case 7: segment = 0xb5; break; // 0b10110101
		case 8: segment = 0x01; break; // 0b00000001
		case 9: segment = 0x21; break; // 0b00100001
		default: segment = 0x49; // 0b01001001 'E' for error
	}
	segment ^= (point & 0x01);
	
	for (i = 0; i < 8; i++)
	{
		if (segment & (0x01 << i))
		{
			v[i] = Bit_SET;
		}
		else
		{
			v[i] = Bit_RESET;
		}
	}
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_0, v[0]);
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, v[1]);
	GPIO_WriteBit(GPIOA, GPIO_Pin_2, v[2]);
	GPIO_WriteBit(GPIOA, GPIO_Pin_3, v[3]);
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, v[4]);
	GPIO_WriteBit(GPIOA, GPIO_Pin_5, v[5]);
	GPIO_WriteBit(GPIOA, GPIO_Pin_6, v[6]);
	GPIO_WriteBit(GPIOA, GPIO_Pin_7, v[7]);
}

void segments_TDM(int num)
{
	int i;
	static int index = -1;
	index = (index + 1) % 4;

	for (i = 0; i < index; i++)
	{
		num /= 10;
	}
	
	digit_select(index);
	digit_show(num % 10, 0);
}

int segments_val = 0;

void task_inc(void* pdata)
{
	while(1)
	{
		segments_val++;
		Delay_ms(100);
	}
}

void task_show(void* pdata)
{
	while(1)
	{
		segments_TDM(segments_val);
		Delay_ms(7);
	}
}

#define STK_Size 100
int Task1_STK[STK_Size];
int Task2_STK[STK_Size];
int Task3_STK[STK_Size];


int main()
{
	GPIO_Configuration();
	OSInit();
	OS_CPU_SysTickInit();
	
	OSTaskCreate(task_inc, (void*)0, (OS_STK*)&Task1_STK[STK_Size-1], 1);
	OSTaskCreate(task_show, (void*)0, (OS_STK*)&Task2_STK[STK_Size-1], 2);
	
	OSStart();
	return 0;
}
