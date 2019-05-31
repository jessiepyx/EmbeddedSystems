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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Delay(int times)
{
	OSTimeDly(OS_TICKS_PER_SEC/1000 * times);
}

void LED0_task(void* pdata)
{
	while(1)
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_SET);
		Delay(200);
		GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_RESET);
		Delay(200);
	}
}

void LED1_task(void* pdata)
{
	while(1)
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
		Delay(200);
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
		Delay(200);
	}
}

#define STK_Size 100
int LED0_Task_STK[STK_Size];
int LED1_Task_STK[STK_Size];
int Task_STK[STK_Size];

int main()
{
	GPIO_Configuration();
	OSInit();
	OS_CPU_SysTickInit();
	OSTaskCreate(LED0_task, (void*)0, (OS_STK*)&LED0_Task_STK[STK_Size-1], 1);
	OSTaskCreate(LED1_task, (void*)0, (OS_STK*)&LED1_Task_STK[STK_Size-1], 2);
	OSStart();
	return 0;
}
