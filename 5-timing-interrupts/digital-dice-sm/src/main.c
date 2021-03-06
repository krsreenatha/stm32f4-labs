#include <stdio.h>

#include "main.h"
#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_rng.h"
#include "misc.h"
#include "stm32f4xx_syscfg.h"


#define ABS(x) ((x)<0 ? -(x) : (x))

#define THRESHOLD    1500

#define STARTING 00
#define WAITING  01
#define SHAKING  10
#define DISPLAY  11

uint8_t state = STARTING;

/* The states are:
STARTING – during initialization
WAITING – waiting for first shake
SHAKING – while shaking the board
DISPLAYING – the result is being displayed on the LEDs
*/

const uint16_t LEDS = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

uint32_t bounceCounter = 0;
float BOUNCE_DELAY = 1000;
int flage;

static void initSystick();
void initAccelerometer();
void initLeds();
void initRng();

void readAxes();
uint8_t readSPI(uint8_t);
void writeSPI(uint8_t, uint8_t);

int8_t isShaking();

void showRandomNumber();

typedef struct {
	int16_t X;
	int16_t Y;
	int16_t Z;
} AccelerometerDataStruct;

AccelerometerDataStruct dat;
AccelerometerDataStruct datOld;

void initSystick() {
	// You can change the frequency of the SysTick interrupt if you want
	SysTick_Config(SystemCoreClock/1000);

}
 
void SysTick_Handler(void) { 

    switch(state) {
    case STARTING:
        // Fill in any necessary actions here
	state=STARTING;
        break;
    case WAITING: 
        // Fill in any necessary actions here
	if(isShaking()==1){
		state=SHAKING;	
		
	}else{
		state=WAITING;
	}
        break;
    case DISPLAY:
        // Fill in any necessary actions here
	if(isShaking()==0 && flage ==1){
		showRandomNumber();
		flage=0;
		state=DISPLAY;
	}else if(isShaking()==0 && flage ==0){
		state=DISPLAY;
	}else{
		state=SHAKING;
	}
        break;
    case SHAKING:
        // Fill in any necessary actions here
	if(isShaking()==1){
		state=SHAKING;
		flage=0;
	}else if(isShaking()==0 && flage==100){
		state=DISPLAY;
		flage=1;
	}else {
		flage++;
	}
        break;
    default:
	state = WAITING;
	break;
    }

}
 
int main() {
	
	SystemInit();
	initSystick();
	initRng();
	initAccelerometer();
	initLeds();
	
	state = WAITING;

	while(1);
}


void initRng() {
	
   RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
   RNG_Cmd(ENABLE);

}

void initAccelerometer() {

	// Initialize SPI1 clock and data lines for slave (accelerometer)

	// Enable clock signal to SPI1 and GPIOA peripherals
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);


	// Configure GPIO pins for SPI1 clock and data lines
 	GPIO_InitTypeDef gpio_spi1;
	gpio_spi1.GPIO_OType = GPIO_OType_PP;
	gpio_spi1.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio_spi1.GPIO_Speed = GPIO_Speed_100MHz;
	gpio_spi1.GPIO_Mode = GPIO_Mode_AF;
	gpio_spi1.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &gpio_spi1);

	// Configure GPIO pins to use alternate function for SPI1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

	// Configure SPI peripheral (master)
	SPI_InitTypeDef spi;

	SPI_StructInit(&spi);
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_CPOL = SPI_CPOL_High;
	spi.SPI_CPHA = SPI_CPHA_2Edge;
	spi.SPI_NSS = SPI_NSS_Soft;

	SPI_Init(SPI1, &spi);
	SPI_Cmd(SPI1, ENABLE);

	// Set up SPI chip select/slave select line
	GPIO_InitTypeDef gpio_cs;
	
	// Enable clock for CS GPIO port
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 
	
	// Configure CS pin
	gpio_cs.GPIO_Pin = GPIO_Pin_3;
	gpio_cs.GPIO_Mode = GPIO_Mode_OUT;
	gpio_cs.GPIO_OType = GPIO_OType_PP;
	gpio_cs.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_cs.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOE, &gpio_cs);

	// Set chip select line (high output)
	GPIO_SetBits(GPIOE, GPIO_Pin_3);
	
	uint16_t temp;
	uint8_t tmpreg;

	// Set up configuration register on MEMS accelerometer chip
	temp  = (uint16_t) (LIS3DSH_DATARATE_100);
	temp |= (uint16_t) (LIS3DSH_XYZ_ENABLE);
	temp |= (uint16_t) (LIS3DSH_SELFTEST_NORMAL);
	temp |= (uint16_t) (LIS3DSH_SERIALINTERFACE_4WIRE);
	temp |= (uint16_t) (LIS3DSH_FULLSCALE_2);
	temp |= (uint16_t) (LIS3DSH_FILTER_BW_50 << 8);
	
	tmpreg = (uint8_t) (temp);

	// Write configuration value (lower byte) to MEMS CTRL_REG4 register
	writeSPI(LIS3DSH_CTRL_REG4_ADDR,tmpreg);
	writeSPI(LIS3DSH_CTRL_REG4_ADDR, tmpreg);


	tmpreg = (uint8_t) (temp >> 8);

	// Write configuration value (upper byte) to MEMS CTRL_REG5 register
	writeSPI(LIS3DSH_CTRL_REG5_ADDR,tmpreg);

	writeSPI(LIS3DSH_CTRL_REG5_ADDR, tmpreg);
}


void writeSPI(uint8_t address, uint8_t data){
 
	// set chip select line low (use GPIO_ResetBits)
	 GPIO_ResetBits(GPIOE,GPIO_Pin_3);

	// The SPI_I2S_FLAG_TXE flag gives the status of the transmit buffer
	// register. Check its status with SPI_I2S_GetFlagStatus. 
	// When it is *not* set, send 'address' using SPI1 with SPI_I2S_SendData
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,address);

	// When it is set, send 'address' using SPI1 with SPI_I2S_SendData


	// The SPI_I2S_FLAG_RXNE flag gives the status of the receiver buffer 
	// register. Check its status with SPI_I2S_GetFlagStatus. 
	// When it is set, receive data with SPI_I2S_ReceiveData
	// (recall that SPI *requires* full duplex, so you should always 
	// receive a byte for each byte you send.)
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET);
	address=SPI_I2S_ReceiveData(SPI1);

	// When the transmit buffer flag is *not* set, send 
	// 'data' using SPI1
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,data);
	// When the receive buffer flag is *not* set, receive a byte 

	 
	// When the transmit buffer flag is set, send 
	// 'data' using SPI1

	// When the receive buffer flag is set, receive a byte 
	// over SPI1
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET);
	data=SPI_I2S_ReceiveData(SPI1);

	// set chip select line high (use GPIO_SetBits)
	 GPIO_SetBits(GPIOE,GPIO_Pin_3);

}


 
uint8_t readSPI(uint8_t address){

	uint8_t data;
 
	// set chip select line low (use GPIO_ResetBits)
	 GPIO_ResetBits(GPIOE,GPIO_Pin_3);
	 
	// Read operations require a 7-bit register address and bit 8 set to 1, so 
	// set `address` to `address` ORed with 1000 0000 (0x80 or 128) 
	// (See section 6.2.1 of the data sheet -
	// http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00040962.pdf
	// - for more details)
	address |=(1<<7);
	

	// The SPI_I2S_FLAG_TXE flag gives the status of the transmit buffer
	// register. Check its status with SPI_I2S_GetFlagStatus. 
	// When it is set, send 'address' using SPI1 with SPI_I2S_SendData


	// The SPI_I2S_FLAG_RXNE flag gives the status of the receiver buffer 
	// register. Check its status with SPI_I2S_GetFlagStatus. 
	// When it is set, receive data with SPI_I2S_ReceiveData
	// (recall that SPI *requires* full duplex, so you should always 
	// receive a byte for each byte you send.)


	// When the transmit buffer flag is set, send 
	// '0x00' as a dummy byte using SPI1
	// (recall that SPI *requires* full duplex, so you should always 
	// send a byte for each byte you receive.)
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,address);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,0x00);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET);
	address=SPI_I2S_ReceiveData(SPI1);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET);
	data=SPI_I2S_ReceiveData(SPI1);
	// When the receive buffer flag is *not* set, receive a byte 

	 
	// When the receive buffer flag is set, receive a byte 

	// over SPI1. Save the byte in `data`

	// set chip select line high (use GPIO_SetBits)
	GPIO_SetBits(GPIOE,GPIO_Pin_3);
	return data;
}
 

void readAxes(AccelerometerDataStruct *dat) {

	// We get 1 byte at a time, and each axes data is 2 bytes.
	// We need a total of 6 bytes to deal with all three directions.
	int8_t axesValues[6];
	
	// Use readSPI to read in values from LIS3DSH_OUT_X_L_ADDR into axesValues[0]
	// and LIS3DSH_OUT_X_H_ADDR into axesValues[1]
	axesValues[0]=readSPI(LIS3DSH_OUT_X_L_ADDR);
	axesValues[1]=readSPI(LIS3DSH_OUT_X_H_ADDR);
	// Use readSPI to read in values from LIS3DSH_OUT_Y_L_ADDR into axesValues[2]
	// and LIS3DSH_OUT_Y_H_ADDR into axesValues[3]
	axesValues[2]=readSPI(LIS3DSH_OUT_Y_L_ADDR);	
	axesValues[3]=readSPI(LIS3DSH_OUT_Y_H_ADDR);
	// Use readSPI to read in values from LIS3DSH_OUT_Z_L_ADDR into axesValues[4]
	// and LIS3DSH_OUT_Z_H_ADDR into axesValues[5]
	axesValues[4]=readSPI(LIS3DSH_OUT_Z_L_ADDR);
	axesValues[5]=readSPI(LIS3DSH_OUT_Z_H_ADDR);
	// Use bit shifting to combine the two bytes for each axis into one 16-bit value
	// in the AccelerometerDataStruct that we passed by reference
	dat->X = (int16_t)((axesValues[1]<<8)+axesValues[0]) * LIS3DSH_SENSITIVITY_0_06G;
	dat->Y = (int16_t)((axesValues[3]<<8)+axesValues[2]) * LIS3DSH_SENSITIVITY_0_06G;
	dat->Z = (int16_t)((axesValues[5]<<8)+axesValues[4]) * LIS3DSH_SENSITIVITY_0_06G;

}



void initLeds() {
    // Enable GPIOD Clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    // Set GPIOD pins 12-15 mode to output
	GPIOD->MODER = 0X55000000;

}



void showRandomNumber() {
	GPIO_ResetBits(GPIOD, LEDS);

        /* Wait for the data to appear.*/
        while(RESET == RNG_GetFlagStatus(RNG_FLAG_DRDY)){}
	// Don't let the number be zero
	int n = 0;
 	while (!n) {
		// Use 4 bits of random number to set LEDs
		n = RNG_GetRandomNumber() & 0xf << 12;
	}
	GPIO_SetBits(GPIOD, n);

}

int8_t isShaking() {
	//in order to judge if the board is in shaking state, we need to debounce technology. 
	readAxes(&dat);
	if(ABS(dat.X)>THRESHOLD|ABS(dat.Y)>THRESHOLD|(dat.Z-900)>THRESHOLD){
		return 1;
	}
	else return 0;
}








