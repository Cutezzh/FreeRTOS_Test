#include "./uart/bsp_uart.h"
#include "./led/bsp_led.h"
#include "FreeRTOS.h"
#include "task.h"

static void BSP_Init(void);

int main(void)
{
	BSP_Init();

}

static void BSP_Init(void){

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//”≈œ»º∂0~15
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