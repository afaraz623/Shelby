#include <main.h>
#include <gpio.h>
#include <stdint.h>
#include <dma.h>
#include <usart.h>
#include <stdio.h>

#define BUF_SIZE 64

/***************************** Prototypes ****************************/
extern void SystemClock_Config(void);

uint8_t txbuff[BUF_SIZE] = {0};
uint8_t txempty = 0;


int main(void)
{
    // Reset of all peripherals, Initializes the Flash interface and the Systick
    HAL_Init();

    // Configure the system clock
    SystemClock_Config();

    // Initialize all configured peripherals
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();

    txempty = 1;
    uint32_t counter = 0;
    
    while(1)
    {
        snprintf(txbuff, BUF_SIZE, "Hello from STM32F3: %ld", counter++);
        while(txempty == 0){}
	    HAL_UART_Transmit_DMA(&huart1, txbuff, BUF_SIZE);
		txempty = 0;
        HAL_Delay(1000);
    }
    
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart1)
	{
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		txempty = 1;
	}
}