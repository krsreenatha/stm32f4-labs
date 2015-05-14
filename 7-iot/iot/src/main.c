#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_adc.h"
#include "misc.h"
#include "stm32f4xx_syscfg.h"

#include "rgbled.h"
#include "lightsensor.h"
#include "bluetooth.h"
#include "leds.h"
#include "power.h"
#include "buffer.h"
#include "audioutils.h"

#include <stdio.h>

//define Bluetooth control signals
#define AUTOcontrol 1   //when sending AUTO or not, send 1 first, and 100(RD) second.
#define POWERcontrol 2
#define COLORcontrol 3  // start and end to set R,G,B color
#define VOLcontrol 4

//define the state machine's state
#define POWER 1
#define COLOR 2
#define VOL 3

//define the received volumn control variable
#define vol1 5
#define vol2 6
#define vol3 7
#define vol4 8
#define vol5 9

//define the received auto control signal
#define AUTOon 1
#define AUTOoff 0




/* Use flags to keep track of system state. Set flags in ISRs, then check their 
   values and act accordingly in the main while() loop.                        */
volatile int flags = 0x0;

/* Circular buffer used for incoming Bluetooth data */
buffer rxBuffer;

/* Set up other global variables here              */
volatile int btcontrol = 0;    //control bluetooth to receive control or data value.
volatile uint8_t controlData=10;
volatile uint8_t valueData;

/* reading value from lightsensor */
/* have flash: 3660
 * without flash: 2460
 * black : 2060
 */

volatile int lightsensor;
volatile uint8_t night;

/* implement auto function 
 * autovalue indicates whether the user open auto function
 */
volatile uint8_t autovalue=1;

/*
 * music
 */
volatile uint8_t playmusic = 1;
volatile uint8_t volumn = 120;

/*
 * Bluetooth Status
 */
volatile int bluetoothstatus; 

/*
 * color, R,G,B
 */
uint8_t red,blue,green;
uint8_t end=0;//judge whether the color setting is done

/**
 * @brief main() implements main system logic
 */
int main() {
    
    initBuffer(&rxBuffer);
    /* Initialize everything else that needs initializing here  */

    /* Init bluetooth and USART for bluetooth
    */
    initUsart();
    initBtState();

    /* 
     *  for light sensor, if interrupt triggers
     *  rgbleds lightup.
     */
    initAdc();
    setAdcThresholds(2200, 2100);

    /*
     * Init LEDs
     */
    initLeds();

    /*
     * Init RGBled
     */
    initPwm();

    /*
     * External Interrupt Init
     */
    initWakeupPin();

    /*
     * test functions
     */
     testbt();
     testbuffer();
     testledup();
     testlightsensor();
     testpower();
     testrgbled();
     testmusic();

        
/*night==1 means the night or disable AUTO mode */
    while (1) {
	if(night==1){
		if((playmusic == 1) || flags == VOL){
			playMusic();
			if(flags == VOL){
				if(valueData == vol1){
					setMusicVolume(130);
					flags = 0;
				}else if(valueData == vol2){
					setMusicVolume(170);
					flags = 0;
				}else if(valueData == vol3){
					setMusicVolume(210);
					flags = 0;
				}else if(valueData == vol4){
					setMusicVolume(250);
					flags = 0;
				}
			}
			GPIOD->BSRRL |= (1<<12);
			playmusic = 0;
		}
		if(flags == POWER){
			
			initWakeupPin();
			flags = 0;
			setMusicVolume(0);
			stop();
		}else if(flags == COLOR){
			setColor(red, green, blue);
			playmusic = 1;
			flags = 0;
		}
	}else if(night == 0){                          //it's daytime
		setColor(0, 0, 0);
		if(playmusic == 0){
			GPIO_ResetBits(GPIOD,4);	
			stopMusic();
			GPIOD->BSRRH |= (1<<12);
			playmusic = 1;
		}
	}
    }

}


/**
 * @brief Interrupt handler for user button (used for wakeup from stop mode)
 * 
 * If you have previously called initWakeupPin(), this handler will be
 * triggered when the User button is pressed (rising edge) or other external 
 * interrupt. 
 */
void EXTI0_IRQHandler(void) {
    if(EXTI_GetITStatus(EXTI_Line0) != RESET){
        EXTI_ClearITPendingBit(EXTI_Line0);
	// do something
	SystemInit();
	GPIOD->BSRRH |= (1<<13);
	night = 1;
	playmusic = 1;
     }
} 


/**
 * @brief Interrupt handler for Bluetooth status changes
 * 
 * If you have previously called initBtState(), this handler will be
 * triggered when a Bluetooth connection is made or broken.
 */
void EXTI1_IRQHandler(void) {
    if(EXTI_GetITStatus(EXTI_Line1) != RESET){
      EXTI_ClearITPendingBit(EXTI_Line1);
      	// Do something on Bluetooth connection status change
	bluetoothstatus = btConnected();
	if(bluetoothstatus){
		GPIOD->BSRRL |= (1<<14);
	}else{
		GPIOD->BSRRH |= (1<<14);
	}
    }
} 


/**
 * @brief Interrupt handler for USART1 (Bluetooth) interrupts
 *
 * If you have previously called initUsart(), this handler will receive 
 * data from the RX port and write it to the circular buffer rxBuffer.
 */
void USART1_IRQHandler(void){
  if( USART_GetITStatus(USART1, USART_IT_RXNE) ){
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	write(&rxBuffer, (uint8_t)USART1->DR);
	if(controlData == COLORcontrol){
		if(end==2){
			red = read(&rxBuffer);
			green = read(&rxBuffer);
			blue = read(&rxBuffer);
			controlData = 0;
			btcontrol=0;
			end = 0;
			flags = COLOR;
		}else {
			end ++;
		}
	} else if(btcontrol == 0){
        	controlData = read(&rxBuffer);
		printf("controlData: %d\n",controlData);
		btcontrol = 1;
	}else if(btcontrol == 1){
		valueData = read(&rxBuffer);
		printf("valueData: %d\n",valueData);
		btcontrol = 0;
	}

	if(controlData == AUTOcontrol){	
		if(valueData ==  AUTOon){		
			autovalue = 1;
		}
		else if(valueData == AUTOoff){
			autovalue = 0;		
		}
	}else if(controlData == POWERcontrol){
		flags = POWER;
	}else if(controlData == VOLcontrol){
		flags = VOL;
		printf("flags: %d\n",flags);
	}	
  }
}


/**
 * @brief Interrupt handler for ADC1 interrupts
 *
 * If you have previously called initAdc(), this handler will be called
 * when the light reading is higher than LIGHT_THRESHOLD_HIGH or lower 
 * than LIGHT_THRESHOLD_LOW. 
 */
void ADC_IRQHandler(void) {
    if(ADC_GetITStatus(ADC1, ADC_IT_AWD) != RESET) {
     	ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
	if(autovalue == 1){
		lightsensor=getLightReading();
		if(lightsensor>2700){
			night=0;
		}
		else{
			night=1;
		}
	}else if(autovalue == 0){
		night = 1;
		lightsensor= 2150;
	}
    }
}
