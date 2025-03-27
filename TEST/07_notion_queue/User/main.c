#include "./uart/bsp_uart.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "event_groups.h"
#include "timers.h"

static void AppCreateTask(void);
static void BSP_Init(void);
static void Send_Task(void * param);
static void Receive_Task(void * param);

static void LED_Task(void * param);
void vLEDTimerCallback(TimerHandle_t xTimer);

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t Send_Task_Handle = NULL;
static TaskHandle_t Receive_Task_Handle = NULL;
static TaskHandle_t LED_Task_Handle = NULL;
// �����¼��顢��ʱ��������¼�λ
#define LED_FLASH_BIT (1 << 0)

QueueHandle_t Test_Queue = NULL;
EventGroupHandle_t Test_Event = NULL;
TimerHandle_t Test_Timer = NULL;

int main(void)
{
	BaseType_t xReturn = pdPASS;
	BSP_Init();
	printf("����һ����Ϣ����ʵ��\r\n");
	printf("����key1 �� key2������Ϣ\r\n");	
	printf("���յ���Ϣ���ڴ����ϻ���\r\n");
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
/* ����������� */
	Test_Queue = xQueueCreate((UBaseType_t)4, (UBaseType_t)4);
	if(NULL != Test_Queue) printf("����Test_Queue�ɹ�\r\n");

	// �����¼���
	Test_Event = xEventGroupCreate();
	if(NULL != Test_Event) printf("����Test_Event�ɹ�\r\n");

	// �������δ�����ʱ��������10��
	Test_Timer = xTimerCreate(
							"LEDTimer",                     // ��ʱ������
							pdMS_TO_TICKS(10000),           // 10������
							pdFALSE,                        // ���δ���
							(void *)0,                      // ��ʱ��ID��δʹ�ã�
							vLEDTimerCallback);             // �ص�����
	if(NULL != Test_Timer) printf("����Test_Timer�ɹ�\r\n");

	xReturn = xTaskCreate((TaskFunction_t)Send_Task, 
							(const char*)"Send_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)3,
							(TaskHandle_t*)&Send_Task_Handle);
	if(pdPASS == xReturn) printf("����Send_Task����ɹ�\r\n");
	xReturn = xTaskCreate((TaskFunction_t)Receive_Task, 
							(const char*)"Receive_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)2,
							(TaskHandle_t*)&Receive_Task_Handle);
if(pdPASS == xReturn) printf("����Receive_Task����ɹ�\r\n");

	xReturn = xTaskCreate((TaskFunction_t)LED_Task, 
								(const char*)"LED_Task",
								(uint16_t)512,
								(void*)NULL,
								(UBaseType_t)2,
								(TaskHandle_t*)&LED_Task_Handle);
	if(pdPASS == xReturn) printf("����LED_Task����ɹ�\r\n");
	vTaskDelete(AppTaskCreate_Handle);
	taskEXIT_CRITICAL();
}

static void Receive_Task(void * param){
	BaseType_t xReturn = pdTRUE;
	uint32_t xRqueue;
	while(1){
		xReturn = xQueueReceive(Test_Queue, &xRqueue, portMAX_DELAY);
//		xReturn = xQueueReceive(Test_Queue, &xRqueue, 500);
		if(xReturn == pdTRUE) printf("�������ݳɹ������յ�������Ϊ%d\r\n", xRqueue);
		// ����¼�λ��ֹͣLED��˸
		xEventGroupClearBits(Test_Event, LED_FLASH_BIT);
		// ���ö�ʱ�������¿�ʼ10�뵹��ʱ
		xTimerReset(Test_Timer, 0);
	}
}

static void Send_Task(void * param){
	BaseType_t xReturn = pdPASS;
	uint32_t data1 = 1, data2 = 2;
	while(1){
		if(Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON){
			printf("������Ϣdata1\r\n");
			xReturn = xQueueSend(Test_Queue, &data1, 0);
			if(xReturn == pdPASS)
			printf("���͵�����Ϊ%d\r\n", data1);
		}
		if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON){
			printf("������Ϣdata2\r\n");
			xReturn = xQueueSend(Test_Queue, &data2, 0);
			if(xReturn == pdPASS)
			printf("���͵�����Ϊ%d\r\n", data2);
		}
		vTaskDelay(20);//��Ϊ����������ȼ�������ߣ�����һֱ��ռ��CPU����Ҫ��������������ʱ��
	}
}

// ��ʱ���ص������������¼�λ����LED��˸
void vLEDTimerCallback(TimerHandle_t Test_Timer) {
    xEventGroupSetBits(Test_Event, LED_FLASH_BIT);
}

static void LED_Task(void * param){
	while(1){
	// �ȴ�LED_FLASH_BIT�����ã����Զ������
	xEventGroupWaitBits(Test_Event, LED_FLASH_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
	
	// ��ʼ��˸������ֱ���¼�λ�����
	while ( (xEventGroupGetBits(Test_Event) & LED_FLASH_BIT) != 0 ) {
		LED1_ON;
		vTaskDelay(500);
		LED1_OFF;
		vTaskDelay(500);
	}
	
	// ֹͣ��˸��ر�LED
	LED1_OFF;
		
	}
}

