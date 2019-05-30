/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#define BUFFER_SIZE 100
#define MEM_SIZE 100

#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t header[] = "Jessie's Boot Loader> ";
uint8_t warning[] = "Illegal command or parameters.\r\n";
uint8_t aRxBuffer[BUFFER_SIZE];
uint16_t bufFront = 0;
uint16_t bufRear = 0;
char command[BUFFER_SIZE];
int cmdSize = 0;
unsigned int mem[MEM_SIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
int respond_command(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
	char str[100];
	int cnt = sprintf(str, "Memory starting at address 0x%p, total size: %d\r\n", mem, MEM_SIZE);
	HAL_UART_Transmit(&huart1, (uint8_t*)str, cnt, 0x1FFFFFF);
	
	HAL_UART_Transmit(&huart1, header, sizeof(header)-1, 0x1FFFFFF);
  HAL_UART_Receive_IT(&huart1, aRxBuffer + bufRear, 1);
	/* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		if (bufRear != bufFront)
		{
			char c = aRxBuffer[bufFront];
			if (++bufFront == BUFFER_SIZE)
			{
				bufFront = 0;
			}
			
			int ret;
			switch (c)
			{
				case '\n':
					ret = respond_command();
					cmdSize = 0;
					if (ret == -1)
					{
						HAL_UART_Transmit(&huart1, warning, sizeof(warning)-1, 0x1FFFFFF);
					}
					HAL_UART_Transmit(&huart1, header, sizeof(header)-1, 0x1FFFFFF);
					break;
				case '\b':
					if (cmdSize > 0)
					{
						cmdSize--;
					}
					break;
				default:
					command[cmdSize++] = c;
			}
		}
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

}

/* USART1 init function */
void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);

}

/** Pinout Configuration
*/
void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	char c = aRxBuffer[bufRear];
	HAL_UART_Transmit(huart, (uint8_t*)&c, 1, 0x1FFFFFF);
	
	if (++bufRear == BUFFER_SIZE)
	{
		bufRear = 0;
	}
	
	if (c == '\r')
	{
		c = '\n';
		aRxBuffer[bufRear] = c;
		HAL_UART_Transmit(huart, (uint8_t*)&c, 1, 0x1FFFFFF);
		
		if (++bufRear == BUFFER_SIZE)
		{
			bufRear = 0;
		}
	}
	
	while (HAL_UART_Receive_IT(huart, aRxBuffer + bufRear, 1) != HAL_OK);
}

int respond_command()
{
	char op[5];
	unsigned int addr;
	unsigned int data;
	int cnt = 0;
	
	// get rid of blanks in the front
	while (cnt < cmdSize && (command[cnt] == ' ' || command[cnt] == '\t'))
		cnt++;
	
	if (cmdSize - cnt < 4) // length of op
		return -1;
	
	for (int i = 0; i < 4; i++)
	{
		op[i] = command[cnt++];
	}
	op[4] = '\0';
		
	// peek addr
	if (strcmp(op, "peek") == 0)
	{
		if (cnt > cmdSize || (command[cnt] != ' ' && command[cnt] != '\t')) // need at least one blank
			return -1;
		
		cnt++;
				
		// get rid of additional blanks
		while (cnt < cmdSize && (command[cnt] == ' ' || command[cnt] == '\t'))
			cnt++;
				
		if (cmdSize - cnt < 10) // length of addr
			return -1;
		
		sscanf(command + cnt, "%x", &addr); // get addr
		cnt += 10;
					
		// validate addr
		if (addr < (unsigned int)mem || addr >= (unsigned int)(mem + MEM_SIZE))
			return -1;
		
		// check if there's other char after addr					
		while (cnt < cmdSize)
		{
			if (command[cnt] != ' ' && command[cnt] != '\t' && command[cnt] != '\r')
				return -1;
			cnt++;
		}
						
		char str[100];
		int cnt = sprintf(str, "%x\r\n", *(unsigned int*)addr);
		HAL_UART_Transmit(&huart1, (uint8_t*)str, cnt, 0x1FFFFFF);
		return 0;
	}
	
	// poke addr data
	else if (strcmp(op, "poke") == 0)
	{
		if (cnt > cmdSize || (command[cnt] != ' ' && command[cnt] != '\t')) // need at least one blank
			return -1;
		
		cnt++;
				
		// get rid of additional blanks
		while (cnt < cmdSize && (command[cnt] == ' ' || command[cnt] == '\t'))
			cnt++;
				
		if (cmdSize - cnt < 10) // length of addr
			return -1;
		
		sscanf(command + cnt, "%x", &addr); // get addr
		cnt += 10;
		
		if (cnt > cmdSize || (command[cnt] != ' ' && command[cnt] != '\t')) // need at least one blank
								{HAL_UART_Transmit(&huart1, (uint8_t*)command + cnt, 1, 0x1FFFFFF);	
		return -1;}
		
		cnt++;
				
		// get rid of additional blanks
		while (cnt < cmdSize && (command[cnt] == ' ' || command[cnt] == '\t'))
			cnt++;
				
		if (cmdSize - cnt < 10) // length of data
			return -1;
		
		sscanf(command + cnt, "%x", &data); // get data
		cnt += 10;
		
		// validate addr
		if (addr < (unsigned int)mem || addr >= (unsigned int)(mem + MEM_SIZE))
			return -1;
		
		// check if there's other char after data					
		while (cnt < cmdSize)
		{
			if (command[cnt] != ' ' && command[cnt] != '\t' && command[cnt] != '\r')
				return -1;
			
			cnt++;
		}
		
		*(unsigned int*)addr = data;
		return 0;
	}
	
	return -1;
}
/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
