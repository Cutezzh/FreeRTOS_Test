#include "./uart/bsp_uart.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

static void AppCreateTask(void);
static void BSP_Init(void);
static void Swtmr1_Callback(void * param);
static void Swtmr2_Callback(void * param);

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t Swtmr1_Handle = NULL;
static TaskHandle_t Swtmr2_Handle = NULL;

static uint32_t Timer1_count = 0;
static uint32_t Timer2_count = 0;

int main(void)
{
	BaseType_t xReturn = pdPASS;
	BSP_Init();
	printf("这是一个软件定时器实验\r\n");
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

}

static void AppCreateTask(void){
	taskENTER_CRITICAL();
	Swtmr1_Handle = xTimerCreate((const char *)"auto timer", 
								 (TickType_t)1000, 
								 (UBaseType_t)pdTRUE, 
								 (void *)1, 
								 (TimerCallbackFunction_t)Swtmr1_Callback);
	if(Swtmr1_Handle != NULL) xTimerStart(Swtmr1_Handle, 0);
	Swtmr2_Handle = xTimerCreate((const char *)"auto timer", 
									(TickType_t)5000, 
									(UBaseType_t)pdFALSE, 
									(void *)2, 
									(TimerCallbackFunction_t)Swtmr2_Callback);
	if(Swtmr2_Handle != NULL) xTimerStart(Swtmr2_Handle, 0);
	vTaskDelete(AppTaskCreate_Handle);
	taskEXIT_CRITICAL();
}

static void Swtmr1_Callback(void * param){
	TickType_t tick_num = 0;
	tick_num = xTaskGetTickCount();
	Timer1_count++;
	printf("Swtmr1(周期)回调次数：%d次\ntick周期： %d\n",Timer1_count, tick_num);
}

static void Swtmr2_Callback(void * param){
	TickType_t tick_num = 0;
	tick_num = xTaskGetTickCount();
	Timer2_count++;
	printf("Swtmr2(单次)回调次数：%d次\ntick周期： %d\n",Timer2_count, tick_num);
}

