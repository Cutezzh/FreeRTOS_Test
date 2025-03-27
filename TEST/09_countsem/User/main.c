#include "./uart/bsp_uart.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

static void AppCreateTask(void);
static void BSP_Init(void);
static void Give_Task(void * param);
static void Take_Task(void * param);


static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t Give_Task_Handle = NULL;
static TaskHandle_t Take_Task_Handle = NULL;

SemaphoreHandle_t CountSem_Handle = NULL;

int main(void)
{
	BaseType_t xReturn = pdPASS;
	BSP_Init();
	printf("这是一个计数信号量实验\r\n");
	xReturn = xTaskCreate((TaskFunction_t)AppCreateTask, 
							(const char*)"AppCreateTask",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)1,
							(TaskHandle_t*)&AppTaskCreate_Handle);
	if(pdPASS == xReturn) vTaskStartScheduler();
	else return -1;
	while(1);
}

static void BSP_Init(void){

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//优先级0~15
	LED_GPIO_Config();
	Key_GPIO_Config();
	USART1_Config();

	// LED1_ON;
	// printf("usart1 ok\n");
	// while(1);
}

static void AppCreateTask(void){
	BaseType_t xReturn = pdPASS;
	taskENTER_CRITICAL();

	CountSem_Handle = xSemaphoreCreateCounting(5,0);
	if (NULL != CountSem_Handle)
	printf("CountSem_Handle 计数信号量创建成功!\r\n");

	xReturn = xTaskCreate((TaskFunction_t)Give_Task, 
							(const char*)"Give_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)2,
							(TaskHandle_t*)&Give_Task_Handle);
	if(pdPASS == xReturn) printf("创建Give_Task任务成功\r\n");
	xReturn = xTaskCreate((TaskFunction_t)Take_Task, 
							(const char*)"Take_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)3,
							(TaskHandle_t*)&Take_Task_Handle);
if(pdPASS == xReturn) printf("创建Take_Task任务成功\r\n");
	vTaskDelete(AppTaskCreate_Handle);
	taskEXIT_CRITICAL();
}

static void Take_Task(void * param){
	BaseType_t xReturn = pdPASS;
	while(1){
		if(Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON){
			xReturn = xSemaphoreTake(CountSem_Handle, 0);
			if(xReturn == pdPASS){
				printf("key1 被按下，停车位申请成功\r\n");
			}else{
				printf("key1 被按下，停车位已满，申请失败\r\n");
			}
		}
		vTaskDelay(20);
	}
}

static void Give_Task(void * param){
	BaseType_t xReturn = pdPASS;
	while(1){
		if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON){
			xReturn = xSemaphoreGive(CountSem_Handle);
			if(xReturn == pdPASS){
				printf("key2 被按下，释放一个停车位\r\n");
			}else{
				printf("key2 被按下，无停车位可释放\r\n");
			}
		}
		vTaskDelay(20);
	}
}




