Lab 4: Accelerometer and SPI
============================

Lab Assignment | 4 - Accelerometer
-------------- | -----------------------
Name           | jizhou	
Net ID         | jz1958
Report due     | Wednesday, 11 March 2015


Please answer the following questions:

1) Which GPIO pins are used by the MEMS accelerometer for each 
of the four SPI lines? (Refer to page 40 of the 
[user manual](http://www.st.com/st-web-ui/static/active/en/resource/technical/document/user_manual/DM00039084.pdf) 
for the STM32F4 Discovery board.)

SPI Line         | Port and pin number
---------------- | --------------------
SCK (clock)      | PA5
MOSI             | PA7
MISO             | PA6
CS (chip select) | PE3


2) Read the `initAccelerometer()` function. Explain in your own words
how the SPI interface is configured and used for communication with the accelerometer.
Answer:
there are four parts, the first and second are configuring master's GPIO and SPI, the third part is configuring CS signal, the last part is configuring MEMS accelerometer.

in the first part, firstly, enable the GPIOA and SPI clock which control MISO, MOSI, SCL signals; secondly, configuring GPIOA pin, with pull-push/ 100MHz speed/ alternate function. the alternate function is for pin's alternate function SPI. thirdly, choosing the alternate function, select PA5, PA6, PA7 as SPI1 function.

in the second part, 
1. Select the BR[2:0] bits to define the serial clock baud rate, the program set it as 32 prescaler.
2. select the CPOL and CPHA bits to define one of the four relationships between the data transfer and the serial clock. the program set CPOL 0x00, CPHA 0x00, so for the slave, the position of capture stable is rising edge of the clock.
3. Set the DFF bit to define 9 or 16 bit data frame format. in this case, we use 8-bit data frame.
4. Configure the LSBFIRST bit in the SPI_CR1 register to define the frame format. we use MSB first.
5. NSS pin in soft or hard mode, in this case, we use NSS soft mode, which means the slave select information is driven internally by the value of the SS bit in the SPI_CR1 register. the external NSS pin remains free for other application uses.
6. SPI direction selection, in this case, we select full duplex mode, that MEMS and CPU could be able to exchange data simutaneously.

in the third part,
we enable the chip select pin, which is PE3. it is configured as output pin, pull-push, pull-up resister, 25MHz speed, and is set.

in the forth part,
a 8-bit data as configuration information for accelerometer sends to MEMS by SPI.

3) What are your zero-G offsets in the X, Y, and Z directions?
Show 10 lines of output from your board when it is 
sitting on a flat surface. Then fill in the table.


```
X: 12 Y: -2 Z: 988
X: 14 Y: -3 Z: 967
X: 12 Y: 0 Z: 986
X: 13 Y: 0 Z: 986
X: 14 Y: -3 Z: 986
X: 11 Y: 0 Z: 986
X: 11 Y: -2 Z: 986
X: 14 Y: -1 Z: 986
X: 14 Y: -2 Z: 985
X: 14 Y: -1 Z: 986
```

Axis         | Offset
------------ | --------------------
X            | 13
Y            | -1
Z            |986

4) Show a screenshot of your `gdb` window at the first step 
_after_ the CS pin is set to low.
answer:
![](https://dl-web.dropbox.com/get/lab4_register.png?_subject_uid=390030028&w=AAAqjs8PquIp3Mh-MlT6DU2_7hgfflNVbJwZNfthKLkDVQ)

5) Show the code you use to light the LEDs according to the board's orientation.

```
if(dat.X<-500){
GPIOD->BSRRL|=(1<<12);
GPIOD->BSRRH|=(1<<13);
GPIOD->BSRRH|=(1<<14);
GPIOD->BSRRH|=(1<<15);
}
if(dat.Y<-500){
GPIOD->BSRRL|=(1<<15);
GPIOD->BSRRH|=(1<<12);
GPIOD->BSRRH|=(1<<13);
GPIOD->BSRRH|=(1<<14);
}
if(dat.Y>500){
GPIOD->BSRRL|=(1<<13);
GPIOD->BSRRH|=(1<<12);
GPIOD->BSRRH|=(1<<14);
GPIOD->BSRRH|=(1<<15);
}
if(dat.X>500){
GPIOD->BSRRL|=(1<<14);
GPIOD->BSRRH|=(1<<12);
GPIOD->BSRRH|=(1<<13);
GPIOD->BSRRH|=(1<<15);
}

```


6) In the `accel-whoami` program, we are reading from a register 
on the device that is configured to return a default value. Based on the value
that is returned, we can identify the device as a LIS3DSH or a LIS302DL.

The SPI Read protocol is described in Section 6.2.1 (Figure 7) of the 
[LIS3DSH datasheet](http://www.st.com/web/en/resource/technical/document/datasheet/DM00040962.pdf).
Create a timing diagram showing exactly what happens (on all four lines) when you call
`readSPI(LIS3DSH_WHO_AM_I_ADDR)`. (The register addresses are defined in `main.h`
and are also given in Table 16 of the 
[LIS3DSH datasheet](http://www.st.com/web/en/resource/technical/document/datasheet/DM00040962.pdf).)
Assume that the accelerometer returns the default value for this register, which is 
given in Section 8.3 of the datasheet.

Insert an image of your timing diagram here.

![timing diagram](https://dl-web.dropbox.com/get/lab5.png?_subject_uid=390030028&w=AAA1fNCLLjZXUs5EuRH1ykKVCcCbnhX4hoJFhu7KDbwljw)







