#include "./uart/bsp_uart.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

static void AppCreateTask(void);
static void BSP_Init(void);
static void High_Task(void * param);
static void Middle_Task(void * param);
static void Low_Task(void * param);

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t High_Task_Handle = NULL;
static TaskHandle_t Middle_Task_Handle = NULL;
static TaskHandle_t Low_Task_Handle = NULL;

SemaphoreHandle_t MuxSem_Handle = NULL;

int main(void)
{
	BaseType_t xReturn = pdPASS;
	BSP_Init();
	printf("这是一个互斥量实验\r\n");
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

	MuxSem_Handle = xSemaphoreCreateMutex();
	if (NULL != MuxSem_Handle)
	printf("MuxSem_Handle 互斥量创建成功!\r\n");
	xReturn = xSemaphoreGive(MuxSem_Handle);//释放信号量

	xReturn = xTaskCreate((TaskFunction_t)High_Task, 
							(const char*)"High_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)3,
							(TaskHandle_t*)&High_Task_Handle);
	if(pdPASS == xReturn) printf("创建High_Task任务成功\r\n");
	xReturn = xTaskCreate((TaskFunction_t)Middle_Task, 
							(const char*)"Middle_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)2,
							(TaskHandle_t*)&Middle_Task_Handle);
if(pdPASS == xReturn) printf("创建Middle_Task任务成功\r\n");
xReturn = xTaskCreate((TaskFunction_t)Low_Task, 
							(const char*)"Low_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)1,
							(TaskHandle_t*)&Low_Task_Handle);
if(pdPASS == xReturn) printf("创建Low_Task任务成功\r\n");
	vTaskDelete(AppTaskCreate_Handle);
	taskEXIT_CRITICAL();
}

static void Low_Task(void * param){
	uint32_t i;
	BaseType_t xReturn = pdPASS;
	while(1){
		printf("低任务获取信号量中。。。。\r\n");
		xReturn = xSemaphoreTake(MuxSem_Handle, portMAX_DELAY);//一直等待信号量
		if(xReturn == pdPASS){
			printf("获取到信号量, 低优先级任务运行中\r\n");
		}
		for(i = 0; i < 100; i++){
			printf("a");
			taskYIELD();
		}
		printf("低任务释放信号量\r\n");
		xReturn = xSemaphoreGive(MuxSem_Handle);
		LED1_TOGGLE;
		vTaskDelay(1000);
	}

}

static void Middle_Task(void * param){

	while(1){
		printf("中优先级任务运行中\r\n");
		vTaskDelay(1000);
	}
}

static void High_Task(void * param){
	BaseType_t xReturn = pdPASS;
	while(1){
		printf("高任务获取信号量中。。。。\r\n");
		xReturn = xSemaphoreTake(MuxSem_Handle, portMAX_DELAY);
		if(xReturn == pdTRUE){
			printf("获取到信号量,高优先级任务运行中\r\n");
		}
		printf("高任务释放信号量\r\n");
		xReturn = xSemaphoreGive(MuxSem_Handle);
		LED1_TOGGLE;
		vTaskDelay(1000);
	}
}


