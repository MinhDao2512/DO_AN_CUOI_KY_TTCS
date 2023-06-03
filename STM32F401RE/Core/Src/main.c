/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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


#include "string.h"
#include "CLCD.h"
#include "stdio.h"

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM4_Init(void);

CLCD_Name LCD1;
uint8_t Count;
char LCD_send[16];
int check_clm(uint16_t pin_var)
{
	int clm;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);		//Set all columns to logic 1 ie connected to Vcc

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);						//set first column to Gnd
	if(HAL_GPIO_ReadPin(GPIOB, pin_var) == 0)
		clm = 1;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);						//set second column to Gnd
	if(HAL_GPIO_ReadPin(GPIOB, pin_var) == 0)
		clm = 2;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);						//set third column to Gnd
	if(HAL_GPIO_ReadPin(GPIOB, pin_var) == 0)
		clm = 3;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);						//set fourth column to Gnd
	if(HAL_GPIO_ReadPin(GPIOB, pin_var) == 0)
		clm = 4;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);		//Set all columns to logic 0 ie connected to GND
	return clm;
}

unsigned char key_press(void)
{
	int clm_var;
	unsigned char key_arr[4][4] = {{'1', '2', '3', 'A'},{'4', '5', '6', 'B'},{'7', '8', '9', 'C'},{'*', '0', '=', 'D'}};
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == 0)
	{
		clm_var = check_clm(GPIO_PIN_0);
		return key_arr[0][clm_var-1];
	}
	else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 0)
	{
		clm_var = check_clm(GPIO_PIN_1);
		return key_arr[1][clm_var-1];
	}
	else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == 0)
	{
		clm_var = check_clm(GPIO_PIN_2);
		return key_arr[2][clm_var-1];
	}
	else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == 0)
	{
		clm_var = check_clm(GPIO_PIN_3);
		return key_arr[3][clm_var-1];
	}
	else
		return '\0';
}

char PW[11]; //Mang de luu mat khau khi nhap tu ban phim
char MK[6] = {'2','2','2','3','3','3'}; //Mang MK cua nguoi dung
int i = 0; //Tang chi so cho mang PW

int so_sanh(char a[],char b[],int n,int m)
{
	if(n > m || n < m)
		return 0;
	else
	{
		for(int j=0;j<6;j++)
		{
			if(a[j] != b[j]) return 0;
		}
		return 1;
	}
}

void ktao()
{
	for(int j=0;j<11;j++) PW[j] = 'z';
}

void Disp_pass_key(char key_var)
{
	if(key_var != '\0')
	{
		if(key_var == 'D'){
			CLCD_SetCursor(&LCD1, 0, 1);
			for(int k = 0;k<i;k++)
			{
				CLCD_WriteChar(&LCD1,' ');
			}
			CLCD_SetCursor(&LCD1, 0, 1);
			i = 0;
		}
		else{
			CLCD_WriteChar(&LCD1,'*');
		}
	}
}

void check(char key_var,int *cnt){
	if(key_var != '\0')
	{
		if(key_var >= '0' && key_var <= '9')
		{
			PW[i] = key_var;
			i++;
		}
		if(key_var == '*')
		{
			if(so_sanh(PW,MK,i,6) == 1)
			{
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "DA MO KHOA!");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, "OPENED!");
				HAL_Delay(1000);
				CLCD_Clear(&LCD1);
				CLCD_WriteString(&LCD1, "XIN CHAO!!!");
				ktao();
				i = 0;
				HAL_Delay(5000);
				//Sao khi da mo khoa thanh cong
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "!!!WELCOME!!!");
				HAL_Delay(1000);
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "ENTER PASSWORD:");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_CursorOn(&LCD1);
				*cnt = 0;
				return;
			}
			else
			{
				*cnt += 1;
				CLCD_Clear(&LCD1);
				CLCD_WriteString(&LCD1, "INCORRECT!");
				ktao();
				sprintf(LCD_send, " Dem:%d", *cnt);
				CLCD_WriteString(&LCD1, LCD_send);
				if(*cnt == 3)
				{
					CLCD_SetCursor(&LCD1, 0, 1);
					CLCD_WriteString(&LCD1, "CANH BAO MUC 3");
				}
				i = 0;
				HAL_Delay(1000);
				if(*cnt < 3){
					CLCD_Clear(&LCD1);
					CLCD_WriteString(&LCD1, "ENTER AGAIN:");
					CLCD_SetCursor(&LCD1, 0, 1);
				}
			}
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}


int main(void)
{
  /* USER CODE BEGIN 1 */
	char var1; //luu ky tu tuong ung tren KeyPAD
	int cnt = 0; //Dem xem so lan da nhap mat khau sai



  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  	HAL_TIM_Base_Start_IT(&htim2);
  	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_SET);
  	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);
  	CLCD_4BIT_Init(&LCD1, 16, 2, CS_GPIO_Port, CS_Pin, EN_GPIO_Port, EN_Pin,
  										D4_GPIO_Port, D4_Pin, D5_GPIO_Port, D5_Pin,
  										D6_GPIO_Port, D6_Pin, D7_GPIO_Port, D7_Pin);
  	CLCD_SetCursor(&LCD1, 0, 0);
  	CLCD_WriteString(&LCD1, "WELCOME!!!");
  	HAL_Delay(2000);
  	CLCD_Clear(&LCD1);
  	CLCD_SetCursor(&LCD1, 0, 0);
  	CLCD_WriteString(&LCD1, "ENTER PASSWORD:");
  	CLCD_SetCursor(&LCD1, 0, 1);
  	CLCD_CursorOn(&LCD1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  var1 = key_press();
	  	Disp_pass_key(var1);
	  	check(var1,&cnt);
	  	if(cnt == 3)
	  	{
	  		HAL_Delay(500);
	  		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
	  		HAL_Delay(3000);
	  		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
	  		CLCD_Clear(&LCD1);
	  		CLCD_SetCursor(&LCD1, 0, 0);
	  		CLCD_WriteString(&LCD1, "CO KE DOT NHAP!");
	  		CLCD_SetCursor(&LCD1, 0, 1);
	  		CLCD_WriteString(&LCD1, "WARNING!!!");
	  		HAL_Delay(2000);
	  		//Sau khi thong bao co ke dot nhap:
	  		CLCD_Clear(&LCD1);
	  		CLCD_SetCursor(&LCD1, 0, 0);
	  		CLCD_WriteString(&LCD1, "WELCOME!!!");
	  	  	HAL_Delay(2000);
	  		CLCD_Clear(&LCD1);
	  		CLCD_SetCursor(&LCD1, 0, 0);
	  		CLCD_WriteString(&LCD1, "ENTER PASSWORD:");
	  		CLCD_SetCursor(&LCD1, 0, 1);
	  		CLCD_CursorOn(&LCD1);
	  		cnt = 0;
	  	}
	  	HAL_Delay(175);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 16000;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 149;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  htim4.Init.Prescaler = 16000;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 199;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB15 PB4 PB5 PB6
                           PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA9 PA10 PA11
                           PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
