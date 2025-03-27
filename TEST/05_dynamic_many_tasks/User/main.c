#include "./uart/bsp_uart.h"
#include "./led/bsp_led.h"
#include "FreeRTOS.h"
#include "task.h"

static void AppCreateTask(void);
static void BSP_Init(void);
static void LED1_Task(void * param);
static void LED2_Task(void * param);

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t LED1_Task_Handle = NULL;
static TaskHandle_t LED2_Task_Handle = NULL;

int main(void)
{
	BaseType_t xReturn = pdPASS;
	BSP_Init();
	printf("这是一个动态多任务创建实验\r\n");
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
	USART1_Config();

	// LED1_ON;
	// printf("usart1 ok\n");
	// while(1);
}

static void AppCreateTask(void){
	BaseType_t xReturn = pdPASS;
	taskENTER_CRITICAL();
	xReturn = xTaskCreate((TaskFunction_t)LED1_Task, 
							(const char*)"LED1_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)2,
							(TaskHandle_t*)&LED1_Task_Handle);
	if(pdPASS == xReturn) printf("创建LED1_Task任务成功\r\n");
	xReturn = xTaskCreate((TaskFunction_t)LED2_Task, 
							(const char*)"LED2_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)3,
							(TaskHandle_t*)&LED2_Task_Handle);
if(pdPASS == xReturn) printf("创建LED2_Task任务成功\r\n");
	vTaskDelete(AppTaskCreate_Handle);
	taskEXIT_CRITICAL();
}

static void LED1_Task(void * param){
	while(1){
		LED1_ON;
		vTaskDelay(500);
		LED1_OFF;
		vTaskDelay(500);
	}
}

static void LED2_Task(void * param){
	while(1){
		LED2_ON;
		vTaskDelay(1000);
		LED2_OFF;
		vTaskDelay(1000);
	}
}

