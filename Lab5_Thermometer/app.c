#include <includes.h>

/*** peripheral configurations & interrupt handlers ***/

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

/************ delay utilities **************/

void Delay_ms(int times)
{
	OSTimeDly(OS_TICKS_PER_SEC/1000 * times);
}

void Delay_us(int times)
{
	unsigned int i;
	for (i = 0; i < times; i++)
	{
	}
}

/************ led segments utilities **************/

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

void segments_TDM_show(int num)
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

/************** DHT11 utilities **************/

#define MAX_TICKS 100000

#define DHT11_OK 0
#define DHT11_NO_CONN 1
#define DHT11_CS_ERROR 2

#define DHT11_PORT GPIOB
#define DHT11_PIN GPIO_Pin_0

void ErrorState(int state)
{
	// infinite loop for suicide
	while(1)
	{
		segments_TDM_show(state);
		Delay_us(4000);
	}
}

void DHT11_Set(int state)
{
	if (state)
	{
		GPIO_WriteBit(DHT11_PORT, DHT11_PIN, Bit_SET);
	}
	else
	{
		GPIO_WriteBit(DHT11_PORT, DHT11_PIN, Bit_SET);
	}
}

void DHT11_Pin_OUT()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
	
	DHT11_Set(1);
}

void DHT11_Pin_IN()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
	
	DHT11_Set(1);
}

uint8_t DHT11_Check()
{
	return GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN);
}

void DHT11_Wait(int state, int place)
{
	int loopCnt = MAX_TICKS;
	while (DHT11_Check() != state)
	{
		if (loopCnt-- == 0)
		{
			ErrorState(1000 + state * 1000 + place);
		}
	}
}

void DHT11_Rst()
{
	// handshake: send
	DHT11_Pin_OUT();
	DHT11_Set(0);
	Delay_ms(25);
	DHT11_Set(1);
	Delay_us(40);
	DHT11_Set(0);

	// handshake: receive
	DHT11_Pin_IN();
}

int val = 10;

uint8_t DHT11_Read_Byte()
{
	int i, cnt;
	uint8_t data = 0;
	for (i = 0; i < 8; i++)
	{
		cnt = 0;
		data <<= 1;
		
		// wait for high signal
		DHT11_Wait(1, ++val);
		
		// compute high duration
		while (DHT11_Check() > 0)
		{
			Delay_us(1);
			cnt++;
		}
		
		// if enough duration, set bit to 1
		data |= cnt > 5;
	}
	return data;
}

uint8_t DHT11_Read_Data(uint8_t *buf)
{
	int i;
	unsigned int cpu_sr;
	
	// enter critical section
	OS_ENTER_CRITICAL();
	
	val = 10;
	
	// handshake
	DHT11_Rst();
	
	// proceed response
	if (DHT11_Check() == 0)
	{
		// low
		DHT11_Wait(1, 2);
		// high
		DHT11_Wait(0, 3);
		// read 40 bits
		for (i = 0; i < 5; i++)
		{
			buf[i] = DHT11_Read_Byte();
		}
		
		DHT11_Pin_OUT();
		
		// exit critical section
		OS_EXIT_CRITICAL();
		
		// checksum
		if (buf[0] + buf[1] + buf[2] + buf[3] == buf[4])
		{
			return DHT11_OK;
		}
		else
		{
			return DHT11_CS_ERROR;
		}
	}
	else
	{
		// exit critical section
		OS_EXIT_CRITICAL();
		
		return DHT11_NO_CONN;
	}
}

uint8_t DHT11_Humidity(uint8_t *buf)
{
	return buf[0];
}

uint8_t DHT11_Temperature(uint8_t *buf)
{
	return buf[2];
}

/**************** uCOS tasks ****************/

int segments_val = 0;

void task_read_DHT11(void* pdata)
{
	uint8_t buf[5];
	int state;
	memset(buf, 0, sizeof(buf));
	
	while (1)
	{
		state = DHT11_Read_Data(buf);
		switch (state)
		{
			case DHT11_CS_ERROR:
				segments_val = 9002;
				break;
			case DHT11_NO_CONN:
				segments_val = 9001;
				break;
			case DHT11_OK:
				segments_val = DHT11_Temperature(buf);
				break;
		}
		Delay_ms(1000);
	}
}

void task_show_segments(void* pdata)
{
	while (1)
	{
		segments_TDM_show(segments_val);
		Delay_ms(7);
	}
}

/**************** main ******************/

#define STK_Size 100
int Task1_STK[STK_Size];
int Task2_STK[STK_Size];

int main()
{
	GPIO_Configuration();
	OSInit();
	OS_CPU_SysTickInit();
	
	OSTaskCreate(task_read_DHT11, (void*)0, (OS_STK*)&Task1_STK[STK_Size-1], 1);
	OSTaskCreate(task_show_segments, (void*)0, (OS_STK*)&Task2_STK[STK_Size-1], 2);
	
	OSStart();
	return 0;
}
