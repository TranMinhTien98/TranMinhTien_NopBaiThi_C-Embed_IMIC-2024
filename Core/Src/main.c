/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DS3231_ADDR 0x68<<1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

osThreadId Task2Handle;
osThreadId Task1Handle;
osThreadId Task3Handle;
osMessageQId myQueue01Handle;
/* USER CODE BEGIN PV */
uint8_t u8_revBuffer[7];
uint8_t u8_tranBuffer[7];
char uartBuf[128];
uint8_t uartRxData;
typedef struct{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t date;
	uint8_t day;
	uint8_t month;
	uint8_t year;
}DS3231_typedef;
DS3231_typedef DS3231_TimeNow;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t BCD2DEC(uint8_t data){
	return (data>>4)*10 + (data&0x0f);
}

uint8_t DEC2BCD(uint8_t data){
	return (data/10)<<4|(data%10);
}

void setTime(int sec, int min, int hour, int day, int date, int month, int year){
	  // Set time
	  u8_tranBuffer[0] = DEC2BCD(sec);
	  u8_tranBuffer[1] = DEC2BCD(min);
	  u8_tranBuffer[2] = DEC2BCD(hour);

	  // Set date
	  u8_tranBuffer[3] = DEC2BCD(day);
	  u8_tranBuffer[4] = DEC2BCD(date);
	  u8_tranBuffer[5] = DEC2BCD(month);
	  u8_tranBuffer[6] = DEC2BCD(year);

	  HAL_I2C_Mem_Write_IT(&hi2c1, DS3231_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, u8_tranBuffer,7);
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2c);

  if(hi2c->Instance == I2C1){
    // Lấy giá trị giây, phút, giờ, ngày, tháng, năm từ bộ đệm
    DS3231_TimeNow.sec = BCD2DEC(u8_revBuffer[0] & 0x7F); // Giữ lại chỉ 7 bit thấp của giây
    DS3231_TimeNow.min = BCD2DEC(u8_revBuffer[1]);
    DS3231_TimeNow.hour = BCD2DEC(u8_revBuffer[2]);

    DS3231_TimeNow.day = BCD2DEC(u8_revBuffer[3]);
    DS3231_TimeNow.date = BCD2DEC(u8_revBuffer[4]);
    DS3231_TimeNow.month = BCD2DEC(u8_revBuffer[5]);
    DS3231_TimeNow.year = BCD2DEC(u8_revBuffer[6]);
  }

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_MemRxCpltCallback could be implemented in the user file
   */
}

int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

//  setTime(00, 34, 22, 3, 12, 02, 25);

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of myQueue01 */
  osMessageQDef(myQueue01, 32, uint8_t);
  myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Task2 */
  osThreadDef(Task2, StartDefaultTask, osPriorityBelowNormal, 0, 128);
  Task2Handle = osThreadCreate(osThread(Task2), NULL);

  /* definition and creation of Task1 */
  osThreadDef(Task1, StartTask02, osPriorityNormal, 0, 128);
  Task1Handle = osThreadCreate(osThread(Task1), NULL);

  /* definition and creation of Task3 */
  osThreadDef(Task3, StartTask03, osPriorityNormal, 0, 128);
  Task3Handle = osThreadCreate(osThread(Task3), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
  HAL_I2C_Mem_Read_IT(&hi2c1, DS3231_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, u8_revBuffer, 7);
  osMessagePut(myQueue01Handle, (uint32_t)&u8_revBuffer, 10);
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	HAL_I2C_Mem_Read_IT(&hi2c1, DS3231_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, u8_revBuffer, 7);
	osMessagePut(myQueue01Handle, (uint32_t)&u8_revBuffer, 10);
	osDelay(50);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the Task1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
      if (HAL_UART_Receive(&huart1, &uartRxData, 1, HAL_MAX_DELAY) == HAL_OK) {
          osMessagePut(myQueue01Handle, uartRxData, osWaitForever);
      }
      osDelay(50);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the Task3 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  uint32_t lastReceivedTick = osKernelSysTick();
  osEvent event;
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  for(;;)
  {
	event = osMessageGet(myQueue01Handle, 0);
	if (event.status == osEventMessage) {
		lastReceivedTick = osKernelSysTick();
		uint8_t receivedChar = event.value.v;

		if (receivedChar == 'D') { // Nếu ấn ký tự D (Date) từ bàn phím: Sẽ hiển thị Ngày:Tháng:Năm
			snprintf(uartBuf, sizeof(uartBuf), "(Pressed D)=> Display only Date, Day: %02d/%02d/%04d (Day: %d)\r\n", // format DD:MM:YYYY
					DS3231_TimeNow.date, DS3231_TimeNow.month, DS3231_TimeNow.year, DS3231_TimeNow.day);
			HAL_UART_Transmit(&huart1, (uint8_t *)uartBuf, strlen(uartBuf), HAL_MAX_DELAY);
		} else if (receivedChar == 'T') { // Nếu ấn ký tự T (Time) từ bàn phím: Sẽ hiển thị Giờ:Phút:Giây
			snprintf(uartBuf, sizeof(uartBuf), "(Pressed T)=> Display only Time: %02d:%02d:%02d\r\n", // format hh:mm:ss
					DS3231_TimeNow.hour, DS3231_TimeNow.min, DS3231_TimeNow.sec);
			HAL_UART_Transmit(&huart1, (uint8_t *)uartBuf, strlen(uartBuf), HAL_MAX_DELAY);
		}
	}

	// Nhận dữ liệu từ Queue thời gian
	event = osMessageGet(myQueue01Handle, 0);
	if (event.status != osEventMessage) {
// Nếu không ấn phím nào kể từ lần ấn gần nhất 2 giây, hiển thị toàn bộ thông tin Ngày:Tháng:Năm Giờ:Phút:Giây
		if (osKernelSysTick() - lastReceivedTick > 2000) {
			snprintf(uartBuf, sizeof(uartBuf), "(No key pressed)=> Auto display Full (Date, Time, Day): %02d/%02d/%04d %02d:%02d:%02d (Day: %d)\r\n",
					DS3231_TimeNow.date, DS3231_TimeNow.month, DS3231_TimeNow.year,
					DS3231_TimeNow.hour, DS3231_TimeNow.min, DS3231_TimeNow.sec, DS3231_TimeNow.day);
			HAL_UART_Transmit(&huart1, (uint8_t *)uartBuf, strlen(uartBuf), HAL_MAX_DELAY);
			lastReceivedTick = osKernelSysTick();
		}
	}

	osDelay(50);
  }
  /* USER CODE END StartTask03 */
}

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
