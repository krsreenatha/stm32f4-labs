/****************************************************************
			Responder-game
	   Real-time embedded system course project

 ----------------------------------------------------------------
	Copyright (c) Minghao Xue, Enze Wu, Jizhou Zhang
		      All rights reserved

 ****************************************************************/



#include <stdio.h>

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_rng.h"
#include "misc.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_i2c.h"
#include "ht16k33.h"


#define IDLE         0
#define DISPLAY      1
#define RESPONDING   2
#define FLASHING     3

#define DEBOUNCE_THRESHOLD 100

/*tables for equations and answers*/
const uint16_t Addendtable1[10] = {one, two, three, four, five, six, seven, eight, nine, one};
const uint16_t Addendtable2[10] = {plus, plus, plus, plus, minus, minus, minus, minus, minus, plus};
const uint16_t Addendtable3[10] = {four, two, one, one, two, three, five, four, six, one};
const uint8_t Answertable[10] ={5,4,4,5,3,3,2,4,3,2};
volatile uint8_t answerCounter[4] = {0,0,0,0};//record answers of each player
volatile uint8_t player_flag[4] = {0,0,0,0};//set to 1 when player start to answer
volatile uint8_t start_flag = 0;//set to 1 when host press the start button
static volatile uint32_t millisecondCounter;
static volatile uint32_t answerTime[4] = {0,0,0,0};//record the time intervial between two times that player press the button

/* The states are:
IDLE - initialization
DISPLAY - display the equation
RESPONDING - players answer the question
FLASHING - flash the led of the competitor who gives the correct answer first
*/
uint8_t state = IDLE;//initial state of system is idle
uint8_t random_n = 0;//generated by rng and used as index to choose question
/*used for debounce*/
int lastPress0 = 0;
int lastPress1 = 0;
int lastPress2 = 0;
int lastPress3 = 0;

const uint16_t LEDS = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
const uint16_t USER_BUTTONS = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;


void initLeds();
void initRng();
void initButtons();
void initTubes();
void initDisplay();
void displayEquation(uint16_t second_num, uint16_t third_num,uint16_t forth_num);
void flashLed(uint8_t player);
void lightLed(uint8_t player);
void EXTI9_5_IRQHandler();
void SysTick_Handler(void);
void giveRandomNumber();


 
void initSystick() {
	millisecondCounter = 0;
	SysTick_Config(SystemCoreClock /1000);//1ms
}
 
void delayMillis(uint32_t millis) {
	uint32_t target;
	target=millisecondCounter+millis;
	while(millisecondCounter < target);
    
}
 
void SysTick_Handler(void) {
	millisecondCounter++;
	answerTime[0]++;
	answerTime[1]++;
	answerTime[2]++;
	answerTime[3]++;
}


int main() {	
	initSystick();
	initRng();
	initButtons();
	initLeds();
	initTubes();
	initDisplay();//display all 0
	while(1){
		switch(state) {
		case IDLE: 
			/*when start button is pressed, create ramdon number and display the question and system enters DISPLAY state*/
			if (start_flag == 1) {
				giveRandomNumber();
				displayEquation(Addendtable1[random_n], Addendtable2[random_n], Addendtable3[random_n]);
				state = DISPLAY;
				start_flag = 0;
			}
        	break;
		case DISPLAY:
			/*as soon as any player start answering, systems enters RESPONDING state */
			if(player_flag[0]==1 || player_flag[1]==1 || player_flag[2]==1 || player_flag[3]==1){
				state = RESPONDING;
			}
        	break;
		case RESPONDING:;
			/*rolling check the flag of each player and light their Led if they start answering*/
			uint8_t i;
			for (i = 0; i < 4; i++) {
				if(player_flag[i] == 1){
					lightLed(i);
				}
			}
			uint8_t j;
			/*rolling check the time intervial of each player, if the time intervial exceed 1s and answer is right, flash his/her Led and 				system enters FLASHING state*/
        		for (j = 0; j < 4; j++) {
				if(answerCounter[j] == Answertable[random_n] && answerTime[j] > 1000){
					flashLed(j);
					state = FLASHING;
				}
			}	 	
        	break;
		case FLASHING:
		break;
		default:
			state = IDLE;
		break;
    		}
	}
}

void initTubes(){
	I2CInit();
	initHt16();	
}

void initDisplay(){
	I2C_start(I2C1, slave_address<<1, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
	I2C_write(I2C1, ram_address);
  	I2C_write(I2C1, 0);
	I2C_write(I2C1, 0>>8);
	I2C_write(I2C1, zero);
	I2C_write(I2C1, zero>>8);
	I2C_write(I2C1, zero);
	I2C_write(I2C1, zero>>8);
	I2C_write(I2C1, zero);
	I2C_write(I2C1, zero>>8);
	I2C_stop(I2C1);	
}

void initRng() {	
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd(ENABLE);
}

void giveRandomNumber() {
	/* Wait for the data to appear.*/
	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET) {}
		random_n = RNG_GetRandomNumber() % 10;
}

void displayEquation(uint16_t second_num, uint16_t third_num,uint16_t forth_num){
	I2C_start(I2C1, slave_address<<1, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
  	I2C_write(I2C1, ram_address);
	I2C_write(I2C1, 0);
 	I2C_write(I2C1, 0>>8);
 	I2C_write(I2C1, second_num);
 	I2C_write(I2C1, second_num>>8);
 	I2C_write(I2C1, third_num);
 	I2C_write(I2C1, third_num>>8);
 	I2C_write(I2C1, forth_num);
 	I2C_write(I2C1, forth_num>>8);
	I2C_stop(I2C1);	
}

/*initLeds
use GPIOD pin 8-11 to light LEDs
pin 8-11 : player1-4
*/
void initLeds() {

	// Use RCC_AHB1PeriphClockCmd to enable the clock on GPIOD
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	// Declare a gpio as a GPIO_InitTypeDef:
	GPIO_InitTypeDef gpio;

        // Call GPIO_StructInit, passing a pointer to gpio
	// This resets the GPIO port to its default values
	GPIO_StructInit(&gpio);

	// Before calling GPIO_Init, 
	// Use gpio.GPIO_Pin to set the pins you are interested in
	// (you can use the LEDS constant defined above)
	gpio.GPIO_Pin = LEDS | GPIO_Pin_14;  
	// Use gpio.GPIO_Mode to set these pins to output mode 
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	// Now call GPIO_Init with the correct arguments
	GPIO_Init(GPIOD, &gpio);

}

/*initButtons
use GPIOA pin 5-8 as EXTI5-8
*/
void initButtons() {
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // Enable GPIOA clock for User button peripheral on GPIOA
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    // Enable SYSCFG clock for interrupts
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    // Configure PA5-PA8 pin as input
    GPIO_InitStructure.GPIO_Pin = USER_BUTTONS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;		
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // Connect EXTI Lines to PA5-8 pin
    // This is the library function equivalent of running
    // SYSCFG->EXTICR[0] &= SYSCFG_EXTICR1_EXTI0_PA;    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource5);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource6);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource7);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);

    // Connect EXTI Line0 (external interrupt line 0) to PA0 pin
    // This is the library function equivalent of running
    // SYSCFG->EXTICR[0] &= SYSCFG_EXTICR1_EXTI0_PA;    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

    // Configure the interrupt EXTI5 using library functions
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;             
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;            
    EXTI_Init(&EXTI_InitStructure);  
                       
    // Configure the interrupt EXTI6 using library functions
    EXTI_InitStructure.EXTI_Line = EXTI_Line6;             
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;            
    EXTI_Init(&EXTI_InitStructure);      

    // Configure the interrupt EXTI7 using library functions
    EXTI_InitStructure.EXTI_Line = EXTI_Line7;             
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;            
    EXTI_Init(&EXTI_InitStructure);      

    // Configure the interrupt EXTI8 using library functions
    EXTI_InitStructure.EXTI_Line = EXTI_Line8;             
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;            
    EXTI_Init(&EXTI_InitStructure); 

    // Configure the interrupt EXTI0 using library functions
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;             
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;            
    EXTI_Init(&EXTI_InitStructure);       

    // Enable and set priorities for the EXTI0 interrupt in NVIC 
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;                
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;   
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;         
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               
    NVIC_Init(&NVIC_InitStructure);            

    // Enable and set priorities for the EXTI9_5 interrupt in NVIC 
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;                
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;   
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;         
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               
    NVIC_Init(&NVIC_InitStructure);             
}


void EXTI0_IRQHandler(void) {
    // Make sure the interrupt flag is set for EXTI0
    if(EXTI_GetITStatus(EXTI_Line0) != RESET){
		start_flag = 1;
	}
	// Clear the interrupt flag
	EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI9_5_IRQHandler(void) {
	
	if (EXTI_GetITStatus(EXTI_Line5) != RESET){

		//set player1's flag
		player_flag[0] = 1;
		//debouncing using delay
		if(millisecondCounter - lastPress0 > DEBOUNCE_THRESHOLD) {
			lastPress0 = millisecondCounter;
			//if it is first time to press player button or the time interial does not exceed 1s, the answer counter will continue to 				increment
			if(answerTime[0] < 1000 || answerTime[0] == millisecondCounter){
				answerCounter[0]++;
				answerTime[0] = 0;
			}
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line6) != RESET){


		player_flag[1] = 1;
		if(millisecondCounter - lastPress1 > DEBOUNCE_THRESHOLD) {
			lastPress1 = millisecondCounter;
			if(answerTime[1] < 1000 || answerTime[1] == millisecondCounter){
				answerCounter[1]++;
				answerTime[1] = 0;
			}
		}
	}

	if (EXTI_GetITStatus(EXTI_Line7) != RESET){


		player_flag[2] = 1;
		if(millisecondCounter - lastPress2 > DEBOUNCE_THRESHOLD) {
			lastPress2 = millisecondCounter;
			if(answerTime[2] < 1000 || answerTime[2] == millisecondCounter){
				answerCounter[2]++;
				answerTime[2] = 0;
			}
		}
	}

	if (EXTI_GetITStatus(EXTI_Line8) != RESET){


		player_flag[3] = 1;
		if(millisecondCounter - lastPress3 > DEBOUNCE_THRESHOLD) {
			lastPress3 = millisecondCounter;
			if(answerTime[3] < 1000 || answerTime[3] == millisecondCounter){
				answerCounter[3]++;
				answerTime[3] = 0;
			}
		}
	}


	// Clear the interrupt flag
	EXTI_ClearITPendingBit(EXTI_Line5);
	EXTI_ClearITPendingBit(EXTI_Line6);
	EXTI_ClearITPendingBit(EXTI_Line7);
	EXTI_ClearITPendingBit(EXTI_Line8);
}

void lightLed(uint8_t player){	
	if(player == 0) GPIO_SetBits(GPIOD, GPIO_Pin_8);
	if(player == 1) GPIO_SetBits(GPIOD, GPIO_Pin_9);
	if(player == 2) GPIO_SetBits(GPIOD, GPIO_Pin_10);
	if(player == 3) GPIO_SetBits(GPIOD, GPIO_Pin_11);
}

void flashLed(uint8_t player) {	
	if(player == 0){
		while(1){
			delayMillis(100);
			GPIO_ToggleBits(GPIOD, GPIO_Pin_8);
		}
	}
	
	if(player == 1){
		while(1){
			delayMillis(100);
			GPIO_ToggleBits(GPIOD, GPIO_Pin_9);
		}
	}

	if(player == 2){
		while(1){
			delayMillis(100);
			GPIO_ToggleBits(GPIOD, GPIO_Pin_10);
		}
	}
	
	if(player == 3){
		while(1){
			delayMillis(100);
			GPIO_ToggleBits(GPIOD, GPIO_Pin_11);
		}
	}
}
