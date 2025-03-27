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
// 定义事件组、定时器句柄和事件位
#define LED_FLASH_BIT (1 << 0)

QueueHandle_t Test_Queue = NULL;
EventGroupHandle_t Test_Event = NULL;
TimerHandle_t Test_Timer = NULL;

int main(void)
{
	BaseType_t xReturn = pdPASS;
	BSP_Init();
	printf("这是一个消息队列实验\r\n");
	printf("按下key1 或 key2发送消息\r\n");	
	printf("接收到消息会在串口上回显\r\n");
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
/* 创建任务队列 */
	Test_Queue = xQueueCreate((UBaseType_t)4, (UBaseType_t)4);
	if(NULL != Test_Queue) printf("创建Test_Queue成功\r\n");

	// 创建事件组
	Test_Event = xEventGroupCreate();
	if(NULL != Test_Event) printf("创建Test_Event成功\r\n");

	// 创建单次触发定时器，周期10秒
	Test_Timer = xTimerCreate(
							"LEDTimer",                     // 定时器名称
							pdMS_TO_TICKS(10000),           // 10秒周期
							pdFALSE,                        // 单次触发
							(void *)0,                      // 定时器ID（未使用）
							vLEDTimerCallback);             // 回调函数
	if(NULL != Test_Timer) printf("创建Test_Timer成功\r\n");

	xReturn = xTaskCreate((TaskFunction_t)Send_Task, 
							(const char*)"Send_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)3,
							(TaskHandle_t*)&Send_Task_Handle);
	if(pdPASS == xReturn) printf("创建Send_Task任务成功\r\n");
	xReturn = xTaskCreate((TaskFunction_t)Receive_Task, 
							(const char*)"Receive_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)2,
							(TaskHandle_t*)&Receive_Task_Handle);
if(pdPASS == xReturn) printf("创建Receive_Task任务成功\r\n");

	xReturn = xTaskCreate((TaskFunction_t)LED_Task, 
								(const char*)"LED_Task",
								(uint16_t)512,
								(void*)NULL,
								(UBaseType_t)2,
								(TaskHandle_t*)&LED_Task_Handle);
	if(pdPASS == xReturn) printf("创建LED_Task任务成功\r\n");
	vTaskDelete(AppTaskCreate_Handle);
	taskEXIT_CRITICAL();
}

static void Receive_Task(void * param){
	BaseType_t xReturn = pdTRUE;
	uint32_t xRqueue;
	while(1){
		xReturn = xQueueReceive(Test_Queue, &xRqueue, portMAX_DELAY);
//		xReturn = xQueueReceive(Test_Queue, &xRqueue, 500);
		if(xReturn == pdTRUE) printf("接收数据成功，接收到的数据为%d\r\n", xRqueue);
		// 清除事件位，停止LED闪烁
		xEventGroupClearBits(Test_Event, LED_FLASH_BIT);
		// 重置定时器，重新开始10秒倒计时
		xTimerReset(Test_Timer, 0);
	}
}

static void Send_Task(void * param){
	BaseType_t xReturn = pdPASS;
	uint32_t data1 = 1, data2 = 2;
	while(1){
		if(Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON){
			printf("发送消息data1\r\n");
			xReturn = xQueueSend(Test_Queue, &data1, 0);
			if(xReturn == pdPASS)
			printf("发送的数据为%d\r\n", data1);
		}
		if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON){
			printf("发送消息data2\r\n");
			xReturn = xQueueSend(Test_Queue, &data2, 0);
			if(xReturn == pdPASS)
			printf("发送的数据为%d\r\n", data2);
		}
		vTaskDelay(20);//因为这个任务优先级任务最高，所以一直抢占着CPU，需要给其他任务运行时间
	}
}

// 定时器回调函数：设置事件位触发LED闪烁
void vLEDTimerCallback(TimerHandle_t Test_Timer) {
    xEventGroupSetBits(Test_Event, LED_FLASH_BIT);
}

static void LED_Task(void * param){
	while(1){
	// 等待LED_FLASH_BIT被设置（不自动清除）
	xEventGroupWaitBits(Test_Event, LED_FLASH_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
	
	// 开始闪烁，持续直到事件位被清除
	while ( (xEventGroupGetBits(Test_Event) & LED_FLASH_BIT) != 0 ) {
		LED1_ON;
		vTaskDelay(500);
		LED1_OFF;
		vTaskDelay(500);
	}
	
	// 停止闪烁后关闭LED
	LED1_OFF;
		
	}
}

