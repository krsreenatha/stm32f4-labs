#include "stm32f4xx.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_usart.h"
#include "misc.h"
#include "stm32f4xx_syscfg.h"

#define USER_BUTTON GPIO_Pin_0

/**
 * @brief Sets up User button to wake system from STOP mode
 *
 * This function configures the User button (PA0) as a digital 
 * input that triggers an interrupt, which may be used to wake the 
 * system from STOP mode.
 * 
 * You must define an interrupt handler (EXTI0_IRQHandler) to handle
 * this interrupt.
 */
void initWakeupPin() {

    /* Copy in your initButton() function from the `interrupt-button' 
       project in lab 5.                                             */
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // Enable GPIOA clock for User button peripheral on GPIOA
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    // Enable SYSCFG clock for interrupts
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    // Configure PA0 pin as input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;		
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // Connect EXTI Line0 (external interrupt line 0) to PA0 pin
    // This is the library function equivalent of running
    // SYSCFG->EXTICR[0] &= SYSCFG_EXTICR1_EXTI0_PA;    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
    
    // Configure the interrupt using library functions
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;             
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;            
    EXTI_Init(&EXTI_InitStructure);                         
    
    // Enable and set priorities for the EXTI0 interrupt in NVIC 
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;                
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;   
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;         
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               
    NVIC_Init(&NVIC_InitStructure);
    PWR_WakeUpPinCmd(ENABLE);                         
}


/**
 * @brief Put system in sleep mode.
 *
 * When the system is in sleep mode, only the Cortex-M4 core is turned off. 
 * Any interrupt that is acknowledged by the NVIC can wake the system from sleep.
 */
void sleep() {

	__WFI();  

}

/**
 * @brief Put system in stop mode.
 *
 * In stop mode, the Cortex-M4 and peripheral clocks are switched off, but volatile
 * memory is preserved.
 *
 * When the system is in stop mode, it may be woken by a rising edge on the wakeup 
 * pin (PA0, connected to User button). 
 * 
 * After waking from stop mode, the HSE is disabled and you may need to call 
 * SystemInit() again to reconfigure your clocks. 
 * 
 * Also, after stop mode, you may need to reset your board before you can flash 
 * code onto it again. In some cases, you may need to reset the board by shorting
 * BOOT0 and VDD and erasing the flash on the board.
 *
 */
void stop() {

	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);

}

/*
 * Mesurement    State    Measured current(mA)
 *    MCU       Normal        33.22mA
 *    MCU       Sleep 	      15.28mA
 *    MCU       Stop          4.45mA
 * Breadboard  BT discovery   40.1mA
 * Breadboard  BT connected   1.89-4.25mA
 * Breadboard  BT Rx/Tx       19.5mA
 */

/* test for power.c function
 * in order to test power, we need to put sleep() and stop() 
 * put into the main.c and test the current 
 * in order to test power during Bluetooth, we need to test the current
 * in different modes of Bt.
 */
void testpower(){
	initWakeupPin();
	stop();
}
