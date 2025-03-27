#include "./uart/bsp_uart.h"
#include "./led/bsp_led.h"
#include "FreeRTOS.h"
#include "task.h"

/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handle;
static TaskHandle_t LED_Task_Handle;

/* 任务堆栈与控制块 */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];//空闲任务任务堆栈
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];//定时器任务堆栈
static StaticTask_t Idle_Task_TCB;//空闲任务控制块
static StaticTask_t Timer_Task_TCB;//定时器任务控制块

static StackType_t AppTaskCreate_Stack[128];//AppTaskCreate任务堆栈
static StackType_t LED_Task_Stack[128];//LED任务堆栈
static StaticTask_t AppTaskCreate_Task_TCB;//AppTaskCreate任务控制块
static StaticTask_t LED_Task_TCB;//LED任务控制块

/* 板级外设函数声明 */
static void BSP_Init(void);
/* 任务函数声明 */
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
	printf("这是一个静态任务创建实验\r\n");

	AppTaskCreate_Handle = xTaskCreateStatic(	(TaskFunction_t)AppTaskCreate,
												(const char*)"AppTaskCreate",
												(uint32_t)128,
												(void*)NULL,
												(UBaseType_t)3,
												(StackType_t*)AppTaskCreate_Stack,
												(StaticTask_t*)&AppTaskCreate_Task_TCB);

	if(NULL != AppTaskCreate_Handle) vTaskStartScheduler();//如果创建成功，启动调度器
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

static void LED_Task(void * param){
	while(1){
		LED1_ON;
		vTaskDelay(500);
		LED1_OFF;
		vTaskDelay(500);
	}
}

//为了方便管理函数，所有任务创建都放在这个函数里
static void AppTaskCreate(void){
	taskENTER_CRITICAL();//进入临界区 就是关闭中断
	/* 创建LED函数 */
	LED_Task_Handle = xTaskCreateStatic(	(TaskFunction_t)LED_Task,
											(const char*)"LEDTask",
											(uint32_t)128,
											(void*)NULL,
											(UBaseType_t)4,
											(StackType_t*)LED_Task_Stack,
											(StaticTask_t*)&LED_Task_TCB);

	if(NULL != LED_Task_Handle) printf("LED_Task创建成功");
	else printf("LED_Task创建失败");
	vTaskDelete(AppTaskCreate_Handle);//删除任务
	taskEXIT_CRITICAL();
}

/* 获取空闲任务的任务堆栈和任务控制块内存 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
								   StackType_t **ppxIdleTaskStackBuffer, 
								   uint32_t *pulIdleTaskStackSize){
	*ppxIdleTaskTCBBuffer = &Idle_Task_TCB;//任务控制块内存
	*ppxIdleTaskStackBuffer = Idle_Task_Stack;//任务堆栈内存
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;//任务堆栈大小
}

/* 获取定时器任务的任务堆栈和任务控制块内存 */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
									StackType_t **ppxTimerTaskStackBuffer, 
									uint32_t *pulTimerTaskStackSize){
	*ppxTimerTaskTCBBuffer = &Timer_Task_TCB;
	*ppxTimerTaskStackBuffer = Timer_Task_Stack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH; 
}
