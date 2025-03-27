#include "./uart/bsp_uart.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"
#include "FreeRTOS.h"
#include "task.h"

static void AppCreateTask(void);
static void BSP_Init(void);
static void LED_Task(void * param);
static void KEY_Task(void * param);

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t LED_Task_Handle = NULL;
static TaskHandle_t KEY_Task_Handle = NULL;

int main(void)
{
	BaseType_t xReturn = pdPASS;
	BSP_Init();
	printf("����һ�����񴴽�����ʵ��\r\n");
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
	xReturn = xTaskCreate((TaskFunction_t)LED_Task, 
							(const char*)"LED_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)2,
							(TaskHandle_t*)&LED_Task_Handle);
	if(pdPASS == xReturn) printf("����LED_Task����ɹ�\r\n");
	xReturn = xTaskCreate((TaskFunction_t)KEY_Task, 
							(const char*)"KEY_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)3,
							(TaskHandle_t*)&KEY_Task_Handle);
if(pdPASS == xReturn) printf("����KEY_Task����ɹ�\r\n");
	vTaskDelete(AppTaskCreate_Handle);
	taskEXIT_CRITICAL();
}

static void LED_Task(void * param){
	while(1){
		LED1_ON;
		printf("led�ƿ���\r\n");
		vTaskDelay(500);
		LED1_OFF;
		printf("led�ƹر�\r\n");
		vTaskDelay(500);
	}
}

static void KEY_Task(void * param){
	while(1){
		if(Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON){
			vTaskSuspend(LED_Task_Handle);
			printf("led���񱻹���\r\n");
		}
		if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON){
			vTaskResume(LED_Task_Handle);
			printf("led���񱻻���\r\n");
		}
		vTaskDelay(20);//��Ϊ����������ȼ�������ߣ�����һֱ��ռ��CPU����Ҫ��������������ʱ��
	}
}

