#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

void init();
void loop();

void initLeds();
void initButton();


int main() {
    init();

    do {
        loop();
    } while (1);
}

void init() {
    initLeds();
    initButton();
}

void loop() {
    if ((GPIOA->IDR)&1==1) {  //judge whether the button is pressed
        // Use BSRRL, set bits of GPIOD pins 12-15
	GPIOD->BSRRL |= (1<<12|1<<13|1<<14|1<<15);

    }
    else {
        // Use BSRRH, set bits of GPIOD pins 12-15
	GPIOD->BSRRH |= (1<<12|1<<13|1<<14|1<<15);
    }
}

void initLeds() {
    // Enable GPIOD Clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    // Set GPIOD pins 12-15 mode to output
	GPIOD->MODER = 0X55000000;
}

void initButton() {
    // Enable GPIOA Clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    // Set GPIOA pin 0 mode to input
	GPIOA->MODER &= ~(0x000000FF);
}

