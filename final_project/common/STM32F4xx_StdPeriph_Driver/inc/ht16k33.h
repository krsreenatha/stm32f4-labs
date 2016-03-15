#include "stm32f4xx.h"

#define LED_ON 1
#define LED_OFF 0

#define LED_RED 1
#define LED_YELLOW 2
#define LED_GREEN 3
 


#define HT16K33_BLINK_CMD 0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_BLINK_OFF 0
#define HT16K33_BLINK_2HZ  1
#define HT16K33_BLINK_1HZ  2
#define HT16K33_BLINK_HALFHZ  3

#define HT16K33_CMD_BRIGHTNESS 0xE0

#define SEVENSEG_DIGITS 5

/* some addresses*/
#define osci 0x21
#define ram_address 0x00
#define slave_address  0x70

/* number mapping */
#define N1 0x36
#define N2 0x21
#define Y1 0x00
#define Y2 0xD0
#define C1 0x39
#define C2 0x00

#define zero 0x003f
#define one 0x0006
#define two 0x00DB
#define three 0x00CF
#define four 0x00E6
#define five 0x00ED
#define six 0x00FD
#define seven 0x0007
#define eight 0x00FF
#define nine 0x00E7
#define plus 0x12C0
#define minus 0x00C0

void I2CInit();
void initHt16();
void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
void I2C_write(I2C_TypeDef* I2Cx, uint8_t data);
void I2C_stop(I2C_TypeDef* I2Cx);


