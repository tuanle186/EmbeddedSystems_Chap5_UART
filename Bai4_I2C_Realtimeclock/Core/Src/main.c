/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"
#include "uart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
#include "lab4.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UPDATE_HOURS 	0
#define UPDATE_MINS		1
#define UPDATE_SECS		2
#define CONFIRM			3

// Lab 5
#define MAX_RETRY 3
#define TIMEOUT_PERIOD 10000 // 10 seconds in milliseconds (assumes 50ms timer interval)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t count_led_debug = 0;

// Lab 5
uint8_t retry_count = 0;       // Tracks retry attempts
uint32_t timeout_timer = 0;    // Tracks time since request sent
uint8_t timeout_flag = 0;      // Indicates if waiting for a response
uint8_t update_time_uart_flag = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
void displayTime();
void updateTime();
void update_time_uart();
void clear_buffer();
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
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  lcd_Clear(BLACK);
  updateTime();
  lcd_Fill(0, 250, 240, 320, BLACK);
  lcd_ShowStr(70, 250, "NORMAL MODE", WHITE, BLACK, 24, 0);
  int status = UPDATE_HOURS;
  while (1)
  {
	  // Periodic timer interrupt flag
	  if (flag_timer2) {
		  flag_timer2 = 0;

		  // Other periodic tasks
		  button_Scan();
		  update_state();
		  clock_run();

		  // Check for button press to enter UART update mode
		  update_time_uart();

		  // Monitor timeout if waiting for a response
		  if (update_time_uart_flag) {
			  if (timeout_flag) {
				  timeout_timer += 50; // Assuming the timer interval is 50ms

				  if (timeout_timer >= TIMEOUT_PERIOD) {
					  timeout_timer = 0; // Reset the timer for the next attempt
					  retry_count++;

					  if (retry_count < MAX_RETRY) {
						  // Retry sending the request
						  switch (status) {
							  case UPDATE_HOURS:
								  uart_Rs232SendString("Hours\n");
								  break;
							  case UPDATE_MINS:
								  uart_Rs232SendString("Minutes\n");
								  break;
							  case UPDATE_SECS:
								  uart_Rs232SendString("Seconds\n");
								  break;
						  }
					  } else {
						  // Exceeded retry limit, display error and reset
						  lcd_StrCenter(0, 20, "Error: No response!", RED, BLACK, 16, 0);
						  reset_timeout(); // Reset timeout and retry counters
						  status = UPDATE_HOURS; // Reset state machine to initial state
						  update_time_uart_flag = 0;
					  }
				  }
			  }
		  }
	  }

	  // Process received data if available
	  if (buffer_flag && update_time_uart_flag) {
		  buffer_flag = 0; // Clear the buffer flag

		  // Process data based on the current state
		  switch (status) {
			  case UPDATE_HOURS:
				  if (isInteger(rcv)) {
					  int hour = atoi(rcv);
					  if (hour >= 0 && hour < 24) {
						  ds3231_Write(ADDRESS_HOUR, hour);
						  lcd_ShowStr(20, 160, "Hours updated", GREEN, BLACK, 16, 0);
						  status = UPDATE_MINS; // Proceed to updating minutes
						  reset_timeout();     // Clear timeout after valid response
					  } else {
						  uart_Rs232SendString("Hours\n"); // Resend request for hours
					  }
				  } else {
					  uart_Rs232SendString("Hours\n"); // Resend request for hours
				  }
				  break;

			  case UPDATE_MINS:
				  if (isInteger(rcv)) {
					  int mins = atoi(rcv);
					  if (mins >= 0 && mins < 60) {
						  ds3231_Write(ADDRESS_MIN, mins);
						  lcd_ShowStr(20, 190, "Minutes updated", GREEN, BLACK, 16, 0);
						  status = UPDATE_SECS; // Proceed to updating seconds
						  reset_timeout();     // Clear timeout after valid response
					  } else {
						  uart_Rs232SendString("Minutes\n"); // Resend request for minutes
					  }
				  } else {
					  uart_Rs232SendString("Minutes\n"); // Resend request for minutes
				  }
				  break;

			  case UPDATE_SECS:
				  if (isInteger(rcv)) {
					  int secs = atoi(rcv);
					  if (secs >= 0 && secs < 60) {
						  ds3231_Write(ADDRESS_SEC, secs);
						  lcd_ShowStr(20, 220, "Seconds updated", GREEN, BLACK, 16, 0);
						  status = CONFIRM; // Proceed to confirmation
						  reset_timeout(); // Clear timeout after valid response
					  } else {
						  uart_Rs232SendString("Seconds\n"); // Resend request for seconds
					  }
				  } else {
					  uart_Rs232SendString("Seconds\n"); // Resend request for seconds
				  }
				  break;

			  case CONFIRM:
				  // Final confirmation and success message
				  lcd_StrCenter(0, 20, "Time updated successfully", GREEN, BLACK, 16, 0);
				  reset_timeout(); // Clear timeout and reset retry counters
				  status = UPDATE_HOURS; // Reset state machine for the next update
				  update_time_uart_flag = 0;
				  break;

			  default:
				  break;
		  }
	  }

	  // Trigger sending a request if timeout_flag is not active
	  if (!timeout_flag && update_time_uart_flag) {
		  switch (status) {
			  case UPDATE_HOURS:
				  uart_Rs232SendString("Hours\n");
				  timeout_flag = 1;  // Start the timeout timer
				  timeout_timer = 0; // Reset the timer
				  break;

			  case UPDATE_MINS:
				  uart_Rs232SendString("Minutes\n");
				  timeout_flag = 1;  // Start the timeout timer
				  timeout_timer = 0; // Reset the timer
				  break;

			  case UPDATE_SECS:
				  uart_Rs232SendString("Seconds\n");
				  timeout_flag = 1;  // Start the timeout timer
				  timeout_timer = 0; // Reset the timer
				  break;

			  default:
				  break;
		  }
	  }

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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init(){
	  HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
	  HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
	  HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);
	  timer_init();
	  led7_init();
	  button_init();
	  lcd_init();
	  uart_init_rs232();
	  ds3231_init();
	  setTimer2(50);
}

void test_LedDebug(){
	count_led_debug = (count_led_debug + 1)%20;
	if(count_led_debug == 0){
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
	}
}

void test_7seg(){
	led7_SetDigit(0, 0, 0);
	led7_SetDigit(5, 1, 0);
	led7_SetDigit(4, 2, 0);
	led7_SetDigit(7, 3, 0);
}

void test_button(){
	for(int i = 0; i < 16; i++){
		if(button_count[i] == 1){
			led7_SetDigit(i/10, 2, 0);
			led7_SetDigit(i%10, 3, 0);
		}
	}
}

void updateTime(){
	ds3231_Write(ADDRESS_YEAR, 24);
	ds3231_Write(ADDRESS_MONTH, 11);
	ds3231_Write(ADDRESS_DATE, 15);
	ds3231_Write(ADDRESS_DAY, 6);
	ds3231_Write(ADDRESS_HOUR, 7);
	ds3231_Write(ADDRESS_MIN, 40);
	ds3231_Write(ADDRESS_SEC, 23);
}

uint8_t isButtonUp()
{
    if (button_count[3] == 1)
        return 1;
    else
        return 0;
}

uint8_t isButtonDown()
{
    if (button_count[7] == 1)
        return 1;
    else
        return 0;
}

void displayTime(){
	lcd_ShowIntNum(70, 100, ds3231_hours, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(110, 100, ds3231_min, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(150, 100, ds3231_sec, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(20, 130, ds3231_day+1, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(70, 130, ds3231_date, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(110, 130, ds3231_month, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(150, 130, ds3231_year, 2, YELLOW, BLACK, 24);
}

// Exercise 5 =================================================================

void update_time_uart(){
	if(button_count[12] == 1){
		lcd_StrCenter(0, 20, "Updating hours...", WHITE, BLACK, 16, 0);
		update_time_uart_flag = 1;
	}
}

void clear_buffer() {
	memset(rcv, 0, sizeof(rcv)); // Clear the buffer
	index_buffer = 0; // reset the index
}

/*
int isInteger(const char *str) {
	uint16_t temp = 0;
    // Check for an empty string
    if (*str == '\0') return 0;

    // Optional: handle a leading negative sign
    if (*str == '-') {
        str++;
    }

    // Check each character to see if it's a digit
    while (*str) {
        if (*str < '0' || *str > '9') {
            return 0; // Non-digit character found
        }

        str++;
    }
    return 1; // All characters are digits
}
*/

int isInteger(const char *str) {
    if (*str == '\0') return 0;  // Empty string
    while (*str) {
        if (!isdigit(*str)) return 0;  // Check if each character is a digit
        str++;
    }
    return 1;
}

void reset_timeout() {
    timeout_flag = 0;
    timeout_timer = 0;
    retry_count = 0;
}

void test_receive(){
	lcd_ShowStr(20, 210, rcv, RED, BLACK, 24, 0);
}

// Exercise 5 END =============================================================

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
