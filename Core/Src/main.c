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
uint32_t GreenTaskProfier = 0;
uint32_t RandomTextGeneratorProfier = 0;
uint32_t PeriodicTaskProfiler = 0;
uint32_t SenderTaskProfiler = 0;
uint32_t ReceiverTaskProfiler = 0;
uint32_t InitTaskProfiler = 0;
uint32_t IdleTaskProfiler = 0;
uint32_t GreenLedController_Priority = 0;
uint32_t SuspendMonitor = 0;

/* Task Handles, allow task modification in runtime */
TaskHandle_t GreenLedController_handle = NULL;
TaskHandle_t RandomTextGenerator_handle = NULL;
TaskHandle_t InitTask_Handle = NULL;
TaskHandle_t PeriodicTask_Handle = NULL;
TaskHandle_t SenderTask_Handle = NULL;
TaskHandle_t ReceiverTask_Handle = NULL;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/* Redirect  printf to UART 2*/
int __io_putchar(int ch);
/* Tasks */
void vGreenLedControllerTask( void * pvParameters );
void vRandomTextGeneratorTask( void * pvParameters );
void vInitTask( void * pvParameters );
void vPeriodicTask( void * pvParameters );
/* ================== Queues ==================  */
void vSenderTask( void * pvParameters );
void vReceiverTask( void * pvParameters );
QueueHandle_t yearQueue;

int main(void)
{


  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();

  /* Create queue BEFORE all tasks */
  yearQueue = xQueueCreate( 10, sizeof( uint32_t ) );
  
  if( yearQueue == NULL )
  {
    printf("ERROR: Queue creation failed!\n\r");
    while(1);
  }
  printf("Queue created successfully\n\r");

  /* Tasks Creations */
  xTaskCreate(vGreenLedControllerTask, 
              "Green Led Controller", 
              configMINIMAL_STACK_SIZE, 
              NULL,                           /* Arguments */
              tskIDLE_PRIORITY + 1,
              &GreenLedController_handle);  /* Task Handle */
  
  xTaskCreate(vRandomTextGeneratorTask, 
              "Random Test Generator", 
              configMINIMAL_STACK_SIZE, 
              NULL, /* Arguments */
              tskIDLE_PRIORITY + 1,
              &RandomTextGenerator_handle); /* Task Handle */

  xTaskCreate(vInitTask,
              "Init System",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY + 1,
              &InitTask_Handle); 

  xTaskCreate(vPeriodicTask,
              "Periodic Task",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY + 1,
              &PeriodicTask_Handle);

              /* Queue tasks */
  xTaskCreate(vSenderTask,
              "Sender Task",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY + 1,   
              &SenderTask_Handle );
  
  xTaskCreate(vReceiverTask,
              "Receiver Task",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY + 1,
              &ReceiverTask_Handle );

  printf("All tasks created, starting scheduler...\n\r");
  
  /* Start scheduler */
  vTaskStartScheduler();
  
  /* We should never get here as control is now taken by the scheduler */
  while (1)
  {
    printf("ERROR\n\r");
  }
}

void vSenderTask( void * pvParameters )
{
    uint32_t year = 2024;
    BaseType_t qStatus;

    while(1)
    {    
        SenderTaskProfiler++;
        printf("Sending Year: %lu\n\r", year);
        qStatus = xQueueSend( yearQueue, &year, pdMS_TO_TICKS( 1000 ) );
        
        if( qStatus != pdPASS )
        {
            printf("Failed to send year\n\r");
        }

        year++;
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}

void vReceiverTask( void * pvParameters )
{
    uint32_t receivedYear;
    BaseType_t qStatus;

    while(1)
    {   
        ReceiverTaskProfiler++;
        qStatus = xQueueReceive( yearQueue, &receivedYear, pdMS_TO_TICKS( 1000 ) );

        if( qStatus == pdPASS )
        {
            printf("Received Year: %lu\n\r", receivedYear);
        }
        else
        {
            printf("Failed to receive year\n\r");
        }
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}

void vInitTask( void * pvParameters )
{
    /* As per most tasks, this task is implemented in an infinite loop. */
    /* Initialize system components */
      InitTaskProfiler++;
      printf("System Initialization\n\r");
      vTaskPrioritySet( InitTask_Handle, tskIDLE_PRIORITY + 2 ); /* Increase Task Priority */
      vTaskDelay( pdMS_TO_TICKS( 1000 ) );

      
      vTaskDelete( GreenLedController_handle ); /* Delete Green Led Controller Task */
      
      vTaskDelete(InitTask_Handle); /* Delete Init Task */
    while(1)
    {
        printf("ERROR\n\r");
    }
}

void vPeriodicTask( void * pvParameters )
{
    /* As per most tasks, this task is implemented in an infinite loop. */
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS( 2000 ); /* 2000ms */
    /* Initialize the xLastWakeTime variable with the current time. */
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    { 
        PeriodicTaskProfiler++;

        printf("Periodic Task\n\r");
        /* Wait for the next cycle. */
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}

void vGreenLedControllerTask( void * pvParameters )
{
    /* As per most tasks, this task is implemented in an infinite loop. */
    while(1)
    {
        /* Toggle the LED each 1000ms */
        GreenTaskProfier++;
        GreenLedController_Priority = uxTaskPriorityGet( GreenLedController_handle ); /* Get Task Priority */
        printf("Toggle Green LED\n\rGreenLedTaskPriority = %ld\n\r", GreenLedController_Priority);
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

        SuspendMonitor++;

        if(SuspendMonitor == 5)
        {
            vTaskSuspend(RandomTextGenerator_handle); /* Suspend Random Text Generator Task */
        }
        else if (SuspendMonitor == 10)
        {
            vTaskResume(RandomTextGenerator_handle); /* Resume Random Text Generator Task */
            SuspendMonitor = 0;
        }
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}

void vRandomTextGeneratorTask( void * pvParameters )
{
    /* As per most tasks, this task is implemented in an infinite loop. */
    while(1)
    {
        /* Generate a random text each 500ms */
        RandomTextGeneratorProfier++;
        printf("Random Text \n\r");
        vTaskPrioritySet( RandomTextGenerator_handle, tskIDLE_PRIORITY + 3 ); /* Increase Task Priority */
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

/* Redirect  printf to UART 2*/
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
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
