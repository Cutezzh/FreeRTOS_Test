#include "./uart/bsp_uart.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

static void AppCreateTask(void);
static void BSP_Init(void);
static void LED_Task(void * param);
static void KEY_Task(void * param);

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t LED_Task_Handle = NULL;
static TaskHandle_t KEY_Task_Handle = NULL;

static EventGroupHandle_t Event_Handle = NULL;

int main(void)
{
	BaseType_t xReturn = pdPASS;
	BSP_Init();
	printf("这是一个事件组实验\r\n");
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

	Event_Handle = xEventGroupCreate();
	if(Event_Handle != NULL) printf("事件组创建成功\r\n");
	taskENTER_CRITICAL();
	xReturn = xTaskCreate((TaskFunction_t)LED_Task, 
							(const char*)"LED_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)2,
							(TaskHandle_t*)&LED_Task_Handle);
	if(pdPASS == xReturn) printf("创建LED_Task任务成功\r\n");
	xReturn = xTaskCreate((TaskFunction_t)KEY_Task, 
							(const char*)"KEY_Task",
							(uint16_t)512,
							(void*)NULL,
							(UBaseType_t)3,
							(TaskHandle_t*)&KEY_Task_Handle);
if(pdPASS == xReturn) printf("创建KEY_Task任务成功\r\n");
	vTaskDelete(AppTaskCreate_Handle);
	taskEXIT_CRITICAL();
}

static void LED_Task(void * param){
	EventBits_t r_event;//接收事件组的变量
	while(1){
		r_event = xEventGroupWaitBits(Event_Handle, 0x01 | 0x02, pdTRUE, pdTRUE, portMAX_DELAY);
		if(r_event & (0x01 | 0x02) == (0x01 | 0x02)){
			printf("按键1和2都被按下\r\n");
			LED1_TOGGLE;
		}else{
			printf("事件错误");
		}
	}
}

static void KEY_Task(void * param){
	while(1){
		if(Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON){
			printf("按键1被按下\r\n");
			xEventGroupSetBits(Event_Handle, 0x01);
		}
		if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON){
			printf("按键2被按下\r\n");
			xEventGroupSetBits(Event_Handle, 0x02);
		}
		vTaskDelay(20);//因为这个任务优先级任务最高，所以一直抢占着CPU，需要给其他任务运行时间
	}
}

