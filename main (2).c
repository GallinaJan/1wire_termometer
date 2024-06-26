/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define DQ			DS18B20_Pin					// Definicja pinu dla magistrali 1-Wire
												// Do wyboru:	iButton_Pin - DS1920
												// 				DS18B20_Pin - DS18B20
#define DQ_Port		DS18B20_GPIO_Port			// Definicja portu dla magistrali 1-Wire
												// Do wyboru:	iButton_GPIO_Port - DS1920
												// 				DS18B20_GPIO_Port - DS18B20
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
char SendRSData[20];						// Bufor na dane wysylane przez UART2
char DQ_Value;								// Pomocnicza zmienna do przechowywania wartości z linii DQ
uint8_t Iter = 0;
volatile uint16_t delay_us = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM11_Init(void);
static void MX_TIM10_Init(void);
/* USER CODE BEGIN PFP */
void Write_DQ(uint8_t);		// Funkcja do zapisu lini danych DQ
char Read_DQ(void);			// Funkcja do odczytu lini danych DQ
void Delay_US(uint16_t);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void DS18B20_Init(void);
void write_bit(uint8_t value);
uint8_t read_bit(void);
void wire_write(uint8_t byte);
uint8_t wire_read(void);
void request_ROM(void);
void read_ROM(void);
void skip_ROM(void);
void convert_T(void);
float get_temp(void);
void read_scratchpad(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM11_Init();
  MX_TIM10_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim10);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	 printf("%f\n",get_temp());
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 83;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 65535;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 83;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 0;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DS18B20_Pin|iButton_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DS18B20_Pin */
  GPIO_InitStruct.Pin = DS18B20_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DS18B20_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : iButton_Pin LD2_Pin */
  GPIO_InitStruct.Pin = iButton_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */


int __io_putchar(int ch)
{
  if (ch == '\n') {
    __io_putchar('\r');
  }

  HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

  return 1;
}


// Zapis linii danych magistrali 1-Wire
void Write_DQ(uint8_t DQValue)
{
	if (DQValue == 0) HAL_GPIO_WritePin(DQ_Port,DQ,GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(DQ_Port,DQ,GPIO_PIN_SET);
}
// END - Zapis linii danych

// Odczyt linii danych magistrali 1-Wire
char Read_DQ(void)
{
	return HAL_GPIO_ReadPin(DQ_Port,DQ);
}
// END - Odczyt linii danych

// Opoznienie w us
void Delay_US(uint16_t us) {
	__HAL_TIM_SET_COUNTER(&htim10, 0);
	while (__HAL_TIM_GET_COUNTER(&htim10) < us) {}
}
// END - Opoznienie w us

// Inicjaliacja polaczenia z DS18B20
void DS18B20_Init(void) {
	  int8_t read;
	  Write_DQ(0);
	  Delay_US(480);
	  Write_DQ(1);
	  Delay_US(70);
	  read = Read_DQ();
	  Delay_US(410);
	  if (read == 0) {
		  printf("connected\n");
	  }
	  else {
		  printf("error\n");
	  }
  }
// END - // Inicjaliacja polaczenia z DS18B20

// Wpisywanie zadanej wartosci do Slave
void write_bit(uint8_t value) {
  if (value) {
	Write_DQ(0);
    Delay_US(6);
    Write_DQ(1);
    Delay_US(64);
  } else {
	Write_DQ(0);
	Delay_US(60);
    Write_DQ(1);
    Delay_US(10);
  }
}
// END - Wpisywanie zadanej wartosci do Slave

uint8_t read_bit(void) {
  uint8_t read;
  Write_DQ(0);
  Delay_US(6);
  Write_DQ(1);
  Delay_US(9);
  read = Read_DQ();
  Delay_US(55);
  return read;
}

void wire_write(uint8_t byte){
  int i;
  for (i = 0; i < 8; i++) {
    write_bit(byte & 0x01);
    byte >>= 1;
  }
}

uint8_t wire_read(void){
  uint8_t value = 0;
  int i;
  for (i = 0; i < 8; i++) {
    value >>= 1;
    if (read_bit())
      value |= 0x80;
  }
  return value;
}

void request_ROM(void) {
	wire_write(0x33);
}

void read_ROM(void) {
	uint8_t i;
	for (i = 0; i < 8; i++) {
		printf("0x%x\n\r",wire_read());
	}
}

void skip_ROM(void) {
	wire_write(0xCC);
}

void convert_T(void) {
	wire_write(0x44);
}

void read_scratchpad(void){
	wire_write(0xBE);
}


float get_temp(void) {
	DS18B20_Init();
	skip_ROM();
	convert_T();
	HAL_Delay(1000);
	DS18B20_Init();
	skip_ROM();
	read_scratchpad();

	int i;
	uint8_t scratchpad[9];
	for (i = 0; i < 9; i++){
		scratchpad[i] = wire_read();
	}

	int16_t temp;
	memcpy(&temp, &scratchpad[0], sizeof(temp));
	return temp / 16.0f;
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

