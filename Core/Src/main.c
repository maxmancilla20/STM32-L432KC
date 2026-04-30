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
#include <stdio.h>
#include "main.h"
#include "cmsis_os.h"


UART_HandleTypeDef huart2;
uint32_t HumidityTaskProfiler = 0;
uint32_t PressureTaskProfiler = 0;
uint32_t ReceiverTaskProfiler = 0;
uint32_t InitTaskProfiler = 0;
uint32_t IdleTaskProfiler = 0;


/* Task Handles, allow task modification in runtime */
TaskHandle_t InitTask_Handle = NULL;
TaskHandle_t HumidityTask_Handle = NULL;
TaskHandle_t PressureTask_Handle = NULL;
TaskHandle_t ReceiverTask_Handle = NULL;

/* Complex Queue Data*/
typedef enum
{
  humidy_sensor,
  pressure_sensor
} DataSource_t;

typedef struct
{
  uint16_t value;
  DataSource_t sDataSource;
} Data_t;

static const Data_t xStructsToSend [ 2 ] =
{
  { 50, humidy_sensor },
  { 1013, pressure_sensor }
};

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/* Redirect  printf to UART 2*/
int __io_putchar(int ch);
/* Tasks */
void vInitTask( void * pvParameters );
/* ================== Queues ==================  */
void vHumidityTask( void * pvParameters );
void vPressureTask( void * pvParameters );
void vReceiverTask( void * pvParameters );

int uart2_write(int ch);

QueueHandle_t yearQueue;
QueueHandle_t dataQueue;

int main(void)
{


  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();

  /* Create queue BEFORE all tasks */
  yearQueue = xQueueCreate( 5, sizeof( uint32_t ) );
  dataQueue = xQueueCreate( 10, sizeof( Data_t ) );

  if( yearQueue == NULL )
  {
    printf("ERROR: Queue creation failed!\n\r");
    while(1);
  }

  if( dataQueue == NULL )
  {
    printf("ERROR: Data queue creation failed!\n\r");
    while(1);
  }
  printf("Queue created successfully\n\r");


  /* Tasks Creations */
  xTaskCreate(vInitTask,
              "Init System",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY + 1,
              &InitTask_Handle); 

  /* Queue tasks */
  xTaskCreate(vHumidityTask, 
              "vHumidityTask", 
              configMINIMAL_STACK_SIZE,
              (void *)&(xStructsToSend[0]), 
              tskIDLE_PRIORITY + 2, 
              &HumidityTask_Handle);

  xTaskCreate(vPressureTask, 
              "vPressureTask", 
              configMINIMAL_STACK_SIZE,
              (void *)&(xStructsToSend[1]), 
              tskIDLE_PRIORITY + 2, 
              &PressureTask_Handle);

  xTaskCreate(vReceiverTask, 
              "Receiver Task", 
              configMINIMAL_STACK_SIZE,
              NULL, 
              tskIDLE_PRIORITY + 3,
              &ReceiverTask_Handle);

  printf("HumidityTask_Handle = %p\n\r", HumidityTask_Handle);
  printf("PressureTask_Handle = %p\n\r", PressureTask_Handle);
  printf("ReceiverTask_Handle = %p\n\r", ReceiverTask_Handle);

  /* Start scheduler */
  printf("All tasks created, starting scheduler...\n\r");
  vTaskStartScheduler();
  
  /* We should never get here as control is now taken by the scheduler */
  while (1)
  {
    printf("ERROR\n\r");
  }
}

void vHumidityTask( void * pvParameters )
{
    printf(">>> Humidity Task STARTED\n\r");
    BaseType_t qStatus;

    while(1)
    {    
        HumidityTaskProfiler++;
        //printf(" Humidity: %ld\n\r", *(uint16_t*)pvParameters);
        qStatus = xQueueSend( dataQueue, pvParameters, pdMS_TO_TICKS( 1000 ) );
        
        if( qStatus != pdPASS )
        {
            printf("Failed to send humidity data\n\r");
        }
        
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}

void vPressureTask( void * pvParameters )
{
    printf(">>> Pressure Task STARTED\n\r");
    BaseType_t qStatus;

    while(1)
    {    
        PressureTaskProfiler++;
        //printf("Pressure: %ld\n\r", *(uint16_t*)pvParameters);
        qStatus = xQueueSend( dataQueue, pvParameters, pdMS_TO_TICKS( 1000 ) );
        
        if( qStatus != pdPASS )
        {
            printf("Failed to send pressure data\n\r");
        }

        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}

void vReceiverTask( void * pvParameters )
{
    printf(">>> ReceiverTask STARTED\n\r"); 
    BaseType_t qStatus;
    Data_t receivedData;
    while(1)
    {   
        ReceiverTaskProfiler++;
        qStatus = xQueueReceive( dataQueue, &receivedData, pdMS_TO_TICKS( 1000 ) );

        if( qStatus != pdPASS )
        {
            printf("Failed to receive data\n\r");
        }
        else
        {
            if(receivedData.sDataSource == humidy_sensor)
            {
                printf("Received Humidity: %d\n\r", receivedData.value);
            }
            else if(receivedData.sDataSource == pressure_sensor)
            {
                printf("Received Pressure: %d\n\r", receivedData.value);
            }
        }
        vTaskDelay( pdMS_TO_TICKS( 500 ) );
    }
}

void vInitTask( void * pvParameters )
{
    /* As per most tasks, this task is implemented in an infinite loop. */
    /* Initialize system components */
      InitTaskProfiler++;
      printf("System Initialization\n\r");
      vTaskPrioritySet( InitTask_Handle, tskIDLE_PRIORITY + 2 ); /* Increase Task Priority */
      
      vTaskDelete(NULL); /* Delete Init Task */

    while(1)
    {
        printf("ERROR\n\r");
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}


void vApplicationIdleHook( void )
{
    /* This function will be called on each cycle of the idle task.  It is essential
    that code added to this hook function never attempts to block in any way (for
    example, call xQueueReceive() with a block time specified, or call
    vTaskDelay()).  If the application makes use of the vTaskDelete() API function
    then it is also important that calls to vPortFree() are thread safe, as it is
    likely that the idle task will be responsible for freeing the memory allocated
    by the kernel to any task that has been deleted. */

    IdleTaskProfiler++;

}


int uart2_write(int ch)
{
    while(!(USART2->ISR & UART_FLAG_TXE)){}
    USART2->TDR = (ch & 0xFF);
    return ch;
}

/* Redirect  printf to UART 2*/
int __io_putchar(int ch)
{
	//HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  uart2_write(ch);
	return ch;
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */


/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
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
