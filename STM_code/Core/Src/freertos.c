/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

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
/* USER CODE BEGIN Variables */
/* USER CODE END Variables */

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name       = "defaultTask",
  .stack_size = 128 * 4,
  .priority   = (osPriority_t)osPriorityNormal,
};

/* Definitions for VoteInterface */
osThreadId_t VoteInterfaceHandle;
const osThreadAttr_t VoteInterface_attributes = {
  .name       = "VoteInterface",
  .stack_size = 256 * 4,
  .priority   = (osPriority_t)osPriorityHigh1,
};

/* Definitions for CommManager */
osThreadId_t CommManagerHandle;
const osThreadAttr_t CommManager_attributes = {
  .name       = "CommManager",
  .stack_size = 256 * 4,
  .priority   = (osPriority_t)osPriorityNormal,
};

/* Definitions for VoteQueue */
osMessageQueueId_t VoteQueueHandle;
const osMessageQueueAttr_t VoteQueue_attributes = {
  .name = "VoteQueue"
};

/* Definitions for ConfirmSem */
osSemaphoreId_t ConfirmSemHandle;
const osSemaphoreAttr_t ConfirmSem_attributes = {
  .name = "ConfirmSem"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartVoteTask(void *argument);
void StartCommTask(void *argument);
extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */


/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of ConfirmSem */
  ConfirmSemHandle = osSemaphoreNew(1, 1, &ConfirmSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of VoteQueue */
  VoteQueueHandle = osMessageQueueNew(5, 8, &VoteQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of VoteInterface */
  VoteInterfaceHandle = osThreadNew(StartVoteTask, NULL, &VoteInterface_attributes);

  /* creation of CommManager */
  CommManagerHandle = osThreadNew(StartCommTask, NULL, &CommManager_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */
}


/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}


/* USER CODE BEGIN Header_StartVoteTask */
/**
  * @brief  Function implementing the VoteInterface thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartVoteTask */
void StartVoteTask(void *argument)
{
  /* USER CODE BEGIN StartVoteTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartVoteTask */
}


/* USER CODE BEGIN Header_StartCommTask */
/**
  * @brief  Function implementing the CommManager thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartCommTask */
void StartCommTask(void *argument)
{
  /* USER CODE BEGIN StartCommTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartCommTask */
}


/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */
