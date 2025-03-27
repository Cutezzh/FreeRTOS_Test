#include "./uart/bsp_uart.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

static void AppCreateTask(void);
static void BSP_Init(void);
static void Receive_Task(void * param);
static void Send_Task(void * param);
// static void LED_Task(void * param);
// static void KEY_Task(void * param);

static TaskHandle_t AppTaskCreate_Handle = NULL;
// static TaskHandle_t LED_Task_Handle = NULL;
// static TaskHandle_t KEY_Task_Handle = NULL;
static TaskHandle_t Receive_Task_Handle = NULL;
static TaskHandle_t Send_Task_Handle = NULL;

SemaphoreHandle_t BinarySem_Handle = NULL;

int main(void)
{
	BaseType_t xReturn = pdPASS;
	BSP_Init();
	printf("这是一个二值信号量实验\r\n");
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

	BinarySem_Handle = xSemaphoreCreateBinary();
	if (NULL != BinarySem_Handle)
	printf("BinarySem_Handle 二值信号量创建成功!\r\n");

	xReturn = xTaskCreate((TaskFunction_t)Receive_Task, 
							(const char*)"Receive_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)2,
							(TaskHandle_t*)&Receive_Task_Handle);
	if(pdPASS == xReturn) printf("创建Receive_Task任务成功\r\n");
	xReturn = xTaskCreate((TaskFunction_t)Send_Task, 
							(const char*)"Send_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)3,
							(TaskHandle_t*)&Send_Task_Handle);
if(pdPASS == xReturn) printf("创建Send_Task任务成功\r\n");
// xReturn = xTaskCreate((TaskFunction_t)LED_Task, 
// (const char*)"LED_Task",
// (uint16_t)512,
// (void*)NULL,
// (UBaseType_t)2,
// (TaskHandle_t*)&LED_Task_Handle);
// if(pdPASS == xReturn) printf("创建LED_Task任务成功\r\n");
// xReturn = xTaskCreate((TaskFunction_t)KEY_Task, 
// (const char*)"KEY_Task",
// (uint16_t)512,
// (void*)NULL,
// (UBaseType_t)3,
// (TaskHandle_t*)&KEY_Task_Handle);
// if(pdPASS == xReturn) printf("创建KEY_Task任务成功\r\n");
	vTaskDelete(AppTaskCreate_Handle);
	taskEXIT_CRITICAL();
}

static void Send_Task(void * param){
	BaseType_t xReturn = pdPASS;
	while(1){
		if(Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON){
			xReturn = xSemaphoreGive(BinarySem_Handle);
			if(xReturn == pdPASS){
				printf("二值信号量释放成功\r\n");
			}else{
				printf("二值信号量释放失败\r\n");
			}
		}
		vTaskDelay(20);
	}
}

static void Receive_Task(void * param){
	BaseType_t xReturn = pdPASS;
	while(1){

		xReturn = xSemaphoreTake(BinarySem_Handle, portMAX_DELAY);
		if(xReturn == pdPASS) printf("信号量获取成功\r\n");
		LED1_TOGGLE;

	}
}

// static void LED_Task(void * param){
// 	while(1){
// 		LED1_ON;
// 		printf("led灯开启\r\n");
// 		vTaskDelay(500);
// 		LED1_OFF;
// 		printf("led灯关闭\r\n");
// 		vTaskDelay(500);
// 	}
// }

// static void KEY_Task(void * param){
// 	while(1){
// 		if(Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON){
// 			vTaskSuspend(LED_Task_Handle);
// 			printf("led任务被挂起\r\n");
// 		}
// 		if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON){
// 			vTaskResume(LED_Task_Handle);
// 			printf("led任务被唤醒\r\n");
// 		}
// 		vTaskDelay(20);//因为这个任务优先级任务最高，所以一直抢占着CPU，需要给其他任务运行时间
// 	}
// }

