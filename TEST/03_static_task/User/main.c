#include "./uart/bsp_uart.h"
#include "./led/bsp_led.h"
#include "FreeRTOS.h"
#include "task.h"

/* ������ */
static TaskHandle_t AppTaskCreate_Handle;
static TaskHandle_t LED_Task_Handle;

/* �����ջ����ƿ� */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];//�������������ջ
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];//��ʱ�������ջ
static StaticTask_t Idle_Task_TCB;//����������ƿ�
static StaticTask_t Timer_Task_TCB;//��ʱ��������ƿ�

static StackType_t AppTaskCreate_Stack[128];//AppTaskCreate�����ջ
static StackType_t LED_Task_Stack[128];//LED�����ջ
static StaticTask_t AppTaskCreate_Task_TCB;//AppTaskCreate������ƿ�
static StaticTask_t LED_Task_TCB;//LED������ƿ�

/* �弶���躯������ */
static void BSP_Init(void);
/* ���������� */
static void LED_Task(void * param);
static void AppTaskCreate(void);
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
								   StackType_t **ppxIdleTaskStackBuffer, 
								   uint32_t *pulIdleTaskStackSize);
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
									StackType_t **ppxTimerTaskStackBuffer, 
									uint32_t *pulTimerTaskStackSize);

int main(void)
{
	BSP_Init();
	printf("����һ����̬���񴴽�ʵ��\r\n");

	AppTaskCreate_Handle = xTaskCreateStatic(	(TaskFunction_t)AppTaskCreate,
												(const char*)"AppTaskCreate",
												(uint32_t)128,
												(void*)NULL,
												(UBaseType_t)3,
												(StackType_t*)AppTaskCreate_Stack,
												(StaticTask_t*)&AppTaskCreate_Task_TCB);

	if(NULL != AppTaskCreate_Handle) vTaskStartScheduler();//��������ɹ�������������
	while(1);

}

static void BSP_Init(void){

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//���ȼ�0~15
	LED_GPIO_Config();
	USART1_Config();

	// LED1_ON;
	// printf("usart1 ok\n");
	// while(1);
}

static void LED_Task(void * param){
	while(1){
		LED1_ON;
		vTaskDelay(500);
		LED1_OFF;
		vTaskDelay(500);
	}
}

//Ϊ�˷�����������������񴴽����������������
static void AppTaskCreate(void){
	taskENTER_CRITICAL();//�����ٽ��� ���ǹر��ж�
	/* ����LED���� */
	LED_Task_Handle = xTaskCreateStatic(	(TaskFunction_t)LED_Task,
											(const char*)"LEDTask",
											(uint32_t)128,
											(void*)NULL,
											(UBaseType_t)4,
											(StackType_t*)LED_Task_Stack,
											(StaticTask_t*)&LED_Task_TCB);

	if(NULL != LED_Task_Handle) printf("LED_Task�����ɹ�");
	else printf("LED_Task����ʧ��");
	vTaskDelete(AppTaskCreate_Handle);//ɾ������
	taskEXIT_CRITICAL();
}

/* ��ȡ��������������ջ��������ƿ��ڴ� */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
								   StackType_t **ppxIdleTaskStackBuffer, 
								   uint32_t *pulIdleTaskStackSize){
	*ppxIdleTaskTCBBuffer = &Idle_Task_TCB;//������ƿ��ڴ�
	*ppxIdleTaskStackBuffer = Idle_Task_Stack;//�����ջ�ڴ�
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;//�����ջ��С
}

/* ��ȡ��ʱ������������ջ��������ƿ��ڴ� */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
									StackType_t **ppxTimerTaskStackBuffer, 
									uint32_t *pulTimerTaskStackSize){
	*ppxTimerTaskTCBBuffer = &Timer_Task_TCB;
	*ppxTimerTaskStackBuffer = Timer_Task_Stack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH; 
}
