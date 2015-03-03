#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"


const uint16_t LEDS = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
const uint16_t USER_BUTTON = GPIO_Pin_0;

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

    // Replace the '1' in the next line with the current User button 
    // input status; use the GPIO_ReadInputDataBit function
    if (GPIO_ReadInputDataBit(GPIOA,1)==1) {

        // Use the GPIO_SetBits function to turn on the LEDs
	 GPIO_SetBits(GPIOD,LEDS);

    }
    else {
        // Use the GPIO_ResetBits function to turn on the LEDs

	GPIO_ResetBits(GPIOD,LEDS);
    }
}

void initLeds() {
    // Use RCC_AHB1PeriphClockCmd to enable the clock on GPIOD
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

    // Declare a gpio as a GPIO_InitTypeDef:
        GPIO_InitTypeDef gpio;

    // Call GPIO_StructInit, passing a pointer to gpio
    // This resets the GPIO port to its default values
	GPIO_StructInit(&gpio);

    // Before calling GPIO_Init, 
    // Use gpio.GPIO_Pin to set the pins you are interested in
    // (you can use the LEDS constant defined above)
	gpio.GPIO_Pin=LEDS;
    // Use gpio.GPIO_Mode to set these pins to output mode 
	gpio.GPIO_Mode=1;
    // Now call GPIO_Init with the correct arguments
	GPIO_Init(GPIOD,&gpio);

}

void initButton() {
    // Use the RCC_AHB1PeriphClockCmd to enable the clock on GPIOA
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA,ENABLE);
    // Declare a gpio as a GPIO_InitTypeDef:
        GPIO_InitTypeDef gpio;

    // Call GPIO_StructInit, passing a pointer to gpio
    // This resets the GPIO port to its default values
	GPIO_StructInit(&gpio);
    // Before calling GPIO_Init, 
    // Use gpio.GPIO_Pin to set the pins you are interested in
    // (you can use the USER_BUTTON constant defined above)
	gpio.GPIO_Pin=USER_BUTTON;
    // Use gpio.GPIO_Mode to set this pins to input mode 
	gpio.GPIO_Mode=0;
    // Now call GPIO_Init with the correct arguments
	GPIO_Init(GPIOA,&gpio);
}

