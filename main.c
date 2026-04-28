/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "lcd4.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char keypad_scan(void){
	int row;
	int col;
    char key_map [4][4] = {
    		{'1', '2', '3', 'A'}, //1st row
			{'4', '5', '6', 'B'}, //2nd row
			{'7', '8', '9', 'C'}, //3rd row
			{'*', '0', '#', 'D'}, //4th row
    };

	for (int i = 0; i<4; i++) {
	    HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, SET);
	    HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, SET);
	    HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, SET);
	    HAL_GPIO_WritePin(ROW4_GPIO_Port, ROW4_Pin, SET);
		if (i==0) {
			HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, RESET);
		}else if (i==1) {
		    HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, RESET);
		}else if (i==2) {
		    HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, RESET);
		}else if (i==3) {
		    HAL_GPIO_WritePin(ROW4_GPIO_Port, ROW4_Pin, RESET);
		}
		row = i;
		HAL_Delay(5);
		col = col_scan();
		HAL_Delay(5);

		if (col==0 || col==1 || col==2 || col==3) {
			return key_map[row][col];
		}
	}
	if (col==-1) {
		return '-';
	}
}

int col_scan(void) {
	if (HAL_GPIO_ReadPin(COL1_GPIO_Port, COL1_Pin) == 0) {
		return 0;
	}else if (HAL_GPIO_ReadPin(COL2_GPIO_Port, COL2_Pin) == 0){
		return 1;
    }else if (HAL_GPIO_ReadPin(COL3_GPIO_Port, COL3_Pin) == 0){
	    return 2;
    }else if (HAL_GPIO_ReadPin(COL4_GPIO_Port, COL4_Pin) == 0){
	    return 3;
    }else{
    	return -1;
    }
}

extern void initialise_monitor_handles(void); //

int seconds = 00;	// Variable that holds the seconds.
int minutes = 00;	// Variable that holds the minutes.
int hours = 12;		// Variable that holds the hours.
char time[3] = "AM";	// Variable that holds the time of day.

uint16_t readValue;		// Variable to hold the raw ADC value from the temperature sensor.
float voltage;			// Variable of the converted voltage of the temperature sensor.
float tempC;			// Variable of the temperature of the temperature sensor.
float temp;				// Variable to be printed on the LCD screen.

char setTemp = 'F';		// Variable that holds what the current temperature reading is.

char buffer[30];	// Variable that holds the string for the the LCD screen.

int clock = 0;		//

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
  MX_ADC1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim4);
  initialise_monitor_handles();

  int setHour = 0;		// Used to change clock set up.
  int setMin = 0;		// Used to change clock set up.

  char setKey[3] = {'-', '-'};	// Used to hold user input
  int index = 0;				// Used to reset the user input.
  int code;						// Holds the user numbered input as an integer.

  //LCD to uC connections: RS - PC3, E - PA1, DB4 - PA4, DB5 - PB0, DB6 - PC1, DB7 - PC0
  lcdSetup(GPIOC, GPIO_PIN_3, GPIOA, GPIO_PIN_1, GPIOA, GPIO_PIN_4, GPIOB, GPIO_PIN_0, GPIOC, GPIO_PIN_1, GPIOC, GPIO_PIN_0);
  lcdInit();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  if (clock == 0) {
		  // Temperature sensor
		  HAL_ADC_Start(&hadc1);
		  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		  readValue = HAL_ADC_GetValue(&hadc1); // Gets the value from the temperature sensor.
		  voltage = readValue * (3.3/4095); // Converts the ADC of the temperature sensor into voltage.
		  tempC = voltage / 0.01; //Converts the voltage into Celsius.
		  if (setTemp == 'F') { // Checks to see if it is in Fahrenheit.
			  temp = (tempC * 9/5) + 32; // Converts the Celsius to Fahrenheit.
		  }else if (setTemp == 'C') { // Checks to see if it is in Celsius.
			  temp = tempC; // Sets the temperature to be printed in Celsius.
		  }

		  // LCD screen
		  lcdCommand(lcdClear); // Code to display the time and temperature on the LCD screen.
		  sprintf(buffer,"%d:%02d:%02d %s %.0f%c",hours, minutes, seconds, time, temp, setTemp);
		  lcdString(buffer);
		  clock = 1; // Resets logic to the next second.
	  }



	  // check if you can change seconds = -1 to something else


	  // Time set
	  char key = keypad_scan(); // Scans the key pad for any button pressed.

	  if (key == '*') { // Checks to see if the asterisks has been pressed.
		  HAL_TIM_Base_Stop_IT(&htim4); // Pauses the timer interrupt.
		  while (1) // While statement to stay in the set up clock state.
		  {
			  key = keypad_scan(); // Scans the key pad for any button pressed.
			  HAL_Delay(50); // Slight delay for no hold over input.
			  //printf("%s\n", setKey); // Semihosting testing for key pad press.

			  if (!setHour) { // Displays hour is set mode.
				  lcdCommand(lcdClear);
				  sprintf(buffer,"HOUR %s", setKey);
				  lcdString(buffer);
			  }else if (setHour && !setMin) { // Displays minutes is set mode.
				  lcdCommand(lcdClear);
				  sprintf(buffer,"MIN %s", setKey);
				  lcdString(buffer);
			  }else if (setHour && setMin) { // Displays time of day is set mode.
				  lcdCommand(lcdClear);
				  sprintf(buffer,"AM or PM %s", setKey);
				  lcdString(buffer);
			  }

			  if (key == '#') {
				  code = atoi(setKey); // Converts the the string number into an integer.
				  if (!setHour){ // If block to set the hours.
					  if (1 <= code && code <= 12) { // Checks to see if number is between 1 and 12.
						  hours = code; // If it is, change hours to the new number.
						  setHour = 1; // Logic to go to the next set up step.
					  }else{ // If it isn't, displays an error.
						  lcdCommand(lcdClear);
						  sprintf(buffer,"ERROR");
						  lcdString(buffer);
						  HAL_Delay(500);	// Delay to make sure the user sees the error.

						  //printf("ERROR\n"); // Semihosting testing for error.
					  }
				  }else if (setHour && !setMin) { // If block to set minutes.
					  if (0 <= code && code <= 59) { // Checks to see if the number is between 1 and 59.
						  minutes = code; // If it is change minutes to the new number.
						  setMin = 1; // Logic to go to the next set up step.
						  seconds = -1;	// Resets the seconds.
					  }else{ // If it isn't, displays an error.
						  lcdCommand(lcdClear);
						  sprintf(buffer,"ERROR");
						  lcdString(buffer);
						  HAL_Delay(500);	// Delay to make sure the user sees the error.

						  //printf("ERROR\n"); // Semihosting testing for error.
					  }
				  }else if (setHour && setMin) { // If block to set the time of day.
					  if (code == 1) { // If the user presses 1.
						  strcpy(time,"AM"); // Set the time of day to AM.
						  HAL_TIM_Base_Start_IT(&htim4); // Re-enables to timer.
						  setHour = 0; // Resets set time set logic for later.
						  setMin = 0; // Resets set time set logic for later.
						  index = 0; // Resets index for later.
						  for (int i=0;i<2;i++) { // For loop to clear the user input.
							  setKey[i] = '-'; // Clears array that holds the user input.
						  }
						  break; // Exits set time while loop.
					  }else if (code == 2) { // If the user presses 2.
						  strcpy(time,"PM"); // Set the time of day to PM.
						  HAL_TIM_Base_Start_IT(&htim4); // Re-enables to timer.
						  setHour = 0; // Resets set time set logic for later.
						  setMin = 0; // Resets set time set logic for later.
						  index = 0; // Resets index for later.
						  for (int i=0;i<2;i++) { // For loop to clear the user input.
							  setKey[i] = '-'; // Clears array that holds the user input.
						  }
						  break; // Exits set time while loop.
					  }else{ // If it isn't, displays an error.
						  lcdCommand(lcdClear);
						  sprintf(buffer,"ERROR");
						  lcdString(buffer);
						  HAL_Delay(500);	// Delay to make sure the user sees the error.

						  //printf("ERROR\n"); // Semihosting testing for error.
					  }
				  }
				  index = 0; // Resets index for later.
				  for (int i=0;i<2;i++) { // For loop to clear the user input.
					  setKey[i] = '-'; // Clears array that holds the user input.
				  }
			  }else if (key != '-' && key != '*') { // If block when there is a button pressed.
				  if (index>1) { // If block when too many inputs have been pressed.
					  lcdCommand(lcdClear);	// Displays error message.
					  sprintf(buffer,"ERROR");
					  lcdString(buffer);
					  HAL_Delay(500);	// Delay to make sure the user sees the error.

					  //printf("ERROR\n"); // Semihosting testing for error.

					  index = 0; // Resets index for later.
					  for (int i=0;i<2;i++) { // For loop to clear the user input.
						  setKey[i] = '-'; // Clears array that holds the user input.
					  }
				  }else{ // If statement for no overhang from the set time
					  setKey[index] = key; // Puts the key pressed into an array.
					  index++; // Increments to index for the next key press.
				  }
			  }
		  }
	  }
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

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
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 8399;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 9999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3|ROW4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_4|GREEN_LED_Pin|ROW1_Pin
                          |ROW2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|ROW3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BLUE_PUSH_BUTTON_Pin */
  GPIO_InitStruct.Pin = BLUE_PUSH_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BLUE_PUSH_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 PC3 ROW4_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3|ROW4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA4 GREEN_LED_Pin ROW1_Pin
                           ROW2_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GREEN_LED_Pin|ROW1_Pin
                          |ROW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 ROW3_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|ROW3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : COL3_Pin COL4_Pin */
  GPIO_InitStruct.Pin = COL3_Pin|COL4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : COL2_Pin COL1_Pin */
  GPIO_InitStruct.Pin = COL2_Pin|COL1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) { // Function for the timer interrupt at every second.
	seconds += 1; // increments the seconds every call.
	if (seconds == 60) { // If statement to increment minutes and reset seconds.
		minutes += 1;
		seconds = 0;
	}
	if (minutes == 60) { // If statement to increment hours and reset minutes.
		hours += 1;
		minutes = 0;
	}

	if (hours == 12 & minutes == 00 & seconds == 00) { // If statement for time of day change at 12.
		if (strcmp(time,"AM") == 0) {	// If statements to change between AM and PM.
			strcpy(time,"PM");
		}else if (strcmp(time,"PM") == 0) {
			strcpy(time,"AM");
		}
	}else if (hours == 13) { // If statement to make the hours go to 1 instead of 13.
		hours = 1;
	}

	if (clock == 1) { // Logic for the LCD display and temperature to be updated every second.
		clock = 0;
	}

	//printf("%d:%02d:%02d %s %.0f%c\n",hours, minutes, seconds, time, temp, setTemp); // Semihosting testing for the clock and temperature.
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) { // Interrupt function for the blue push button.
	if (setTemp == 'F') { // If statement to change between Celsius and Fahrenheit.
		setTemp = 'C';
	}else if (setTemp == 'C') {
		setTemp = 'F';
	}
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
#ifdef USE_FULL_ASSERT
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
