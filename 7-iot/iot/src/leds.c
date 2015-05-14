

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#define LEDS  GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15


/**
 * @brief Set up on-board LEDs as GPIO outputs
 *
 * This function initializes the GPIO output pins connected 
 * to the four on-board LEDs.
 */
void initLeds() {
    // Enable GPIOD Clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    // Set GPIOD pins 12-15 mode to output
	GPIOD->MODER = 0X55000000;

}

/**
 * this function provide the test function to light up LEDs
 */
void testledup(){
	initLeds();
	GPIOD->BSRRL |= (1<<12|1<<13|1<<14|1<<15);
}
void testleddown(){
	initLeds();
	GPIOD->BSRRH |= (1<<12|1<<13|1<<14|1<<15);
}
