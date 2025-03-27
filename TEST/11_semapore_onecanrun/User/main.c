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
	printf("����һ��������ʵ��\r\n");
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

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//���ȼ�0~15
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
	printf("MuxSem_Handle �����������ɹ�!\r\n");
	xReturn = xSemaphoreGive(MuxSem_Handle);//�ͷ��ź���

	xReturn = xTaskCreate((TaskFunction_t)High_Task, 
							(const char*)"High_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)3,
							(TaskHandle_t*)&High_Task_Handle);
	if(pdPASS == xReturn) printf("����High_Task����ɹ�\r\n");
	xReturn = xTaskCreate((TaskFunction_t)Middle_Task, 
							(const char*)"Middle_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)2,
							(TaskHandle_t*)&Middle_Task_Handle);
if(pdPASS == xReturn) printf("����Middle_Task����ɹ�\r\n");
xReturn = xTaskCreate((TaskFunction_t)Low_Task, 
							(const char*)"Low_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)1,
							(TaskHandle_t*)&Low_Task_Handle);
if(pdPASS == xReturn) printf("����Low_Task����ɹ�\r\n");
	vTaskDelete(AppTaskCreate_Handle);
	taskEXIT_CRITICAL();
}

static void Low_Task(void * param){
	uint32_t i;
	BaseType_t xReturn = pdPASS;
	while(1){
		printf("�������ȡ�ź����С�������\r\n");
		xReturn = xSemaphoreTake(MuxSem_Handle, portMAX_DELAY);//һֱ�ȴ��ź���
		if(xReturn == pdPASS){
			printf("��ȡ���ź���, �����ȼ�����������\r\n");
		}
		for(i = 0; i < 100; i++){
			printf("a");
			taskYIELD();
		}
		printf("�������ͷ��ź���\r\n");
		xReturn = xSemaphoreGive(MuxSem_Handle);
		LED1_TOGGLE;
		vTaskDelay(1000);
	}

}

static void Middle_Task(void * param){

	while(1){
		printf("�����ȼ�����������\r\n");
		vTaskDelay(1000);
	}
}

static void High_Task(void * param){
	BaseType_t xReturn = pdPASS;
	while(1){
		printf("�������ȡ�ź����С�������\r\n");
		xReturn = xSemaphoreTake(MuxSem_Handle, portMAX_DELAY);
		if(xReturn == pdTRUE){
			printf("��ȡ���ź���,�����ȼ�����������\r\n");
		}
		printf("�������ͷ��ź���\r\n");
		xReturn = xSemaphoreGive(MuxSem_Handle);
		LED1_TOGGLE;
		vTaskDelay(1000);
	}
}


