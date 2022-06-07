#include "main.h"
#include "gpio.h"
#include <stdint.h>

/***************************** Prototypes ****************************/
extern void SystemClock_Config(void);

int main(void)
{
    // Reset of all peripherals, Initializes the Flash interface and the Systick
    HAL_Init();

    // Configure the system clock
    SystemClock_Config();

    // Initialize all configured peripherals
    MX_GPIO_Init();

    uint16_t led_pins[] = {LED1_Pin, LED2_Pin, LED3_Pin, LED4_Pin, LED5_Pin, LED6_Pin, LED7_Pin, LED8_Pin};

    while (1)
    {
        for (int i = 0; i < (sizeof(led_pins) / sizeof(uint16_t)); ++i)
        {
            HAL_GPIO_TogglePin(GPIOE, led_pins[i]);
            HAL_Delay(1000);
        }
    }
}