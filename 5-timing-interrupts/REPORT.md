Lab 5: Timing and Interrupts
============================

Lab Assignment | 5 - Timing
-------------- | -----------------------
Name             | jizhou	
Net ID           | jz1958
Report due       | Thursday, 9 April 2015


Please answer the following questions:

1) The following lines of code can be found in the `main.c` source file for the `interrupt-button` exercise.

Comment *each* line of code, explaining its purpose. (Not just the semantics of what it does, but what its *purpose* is.)

```
SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0); // select GPIOA, pin0 as input external interrupt pin.

// Configure the interrupt using library functions
EXTI_InitStructure.EXTI_Line = EXTI_Line0; // enable external interrupt line 0 including PA0, PB0 to PK0
EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  // choose external interrupt modes(interrupt or event) as interrupt mode
EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // choose trigger conditions(rising,falling or rising and falling) as rising and falling edge.
EXTI_InitStructure.EXTI_LineCmd = ENABLE; // enable external interrupt on PA0
EXTI_Init(&EXTI_InitStructure);   // initiate the configurated initStructure 

// Enable and set priorities for the EXTI0 interrupt in NVIC
NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; // specifies the IRQ channel to be enabled or disabled. the IRQ channel includes many optionns, such as EXTI0_IRQn, I2C1_ER_IRQn, SPI1_IRQn, USART1_IRQn etc.
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; // specifies the preemption priority for the IRQ channel. this parameter can be a balue between 0 and 15.
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; // specifies the subpriority level for the IRQ channel.
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //the IRQ channel defined is enable.
NVIC_Init(&NVIC_InitStructure);  //pass the value into NVIC_Init function to initiate.
```

2) Show the output of `print risingCounter` and `print risingCounter` in `gdb` that demonstrates switch bouncing.


```
(gdb) print risingCounter
$1 = 28
(gdb) print fallingCounter
$2 = 21

```

3) Explain a method of switch debouncing using extra circuitry.
explain: one method of debouncing is to add RC circuit. R represents resistor, C represents capacitor. when the button is pressed, the capacitor is in discharging status; when the button is released, the capacitor is in charging status. Thus, when the button is pressed, without RC circuit, the voltage bounces several times before stable. However with RC circuit, expecially capacitor characteristic, the debounce can be implemented, since the changing voltage is eliminated by the discharging capacitor. Eventually, the voltage is gradually changed from 0 to 1 or 1 to 0.

4) Explain a method of switch debouncing in software.
explain: one method of debouncing in software is to mimic the RC circuit. Based on the equation y_old = x_new*0.25 + y_old*0.74, at the beginning the parameters y_old equals to 0, x_new equals to 0x3F. Then when bounce is tested each time, in other word, the input registering bit correponding to that button is set, then the equation is doing once. So in one button pressed, there are several times bouncing tested which means the equation is doing several times and y_old is gradually incrementing. This mechanism is similar to capacitor's characteristic. Besides, we have to set a threshold, which output 1 is y_old is larger than threshold, otherwise output 0. Thus, we can get the right judge for the button pressed times.

5) Explain the circumstances that trigger the bug in the
`interrupt-button` program where the LEDs stay lit even
when the button is released. What conditions will cause
this bug to occur?
explain: considering the following scenario, when button is pressed, then interrupt is triggered, executing the ISR. However, because there is for-loop in the ISR, that after lighting up the LEDs, the PC need to execute 10000 times reading data from GPIO. It's rational that in this for-loop period, the button is released, and then another interrupt triggered. as now, the PC is in the first interrupt that the second one is discarded. Besides, cause we change if-if to if-else if, which means that in the ISR when judging the first if and no need to judge the else-if. Essentially, the circuit skip to judge if the button is released.

6) In the `interrupt-button` program, in the ISR, we
have the following lines of code:

```
if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {
  GPIO_SetBits(GPIOD, LEDS);
}
if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {
  GPIO_ResetBits(GPIOD, LEDS);
}
```

Will these two `if` blocks always be evaluated in the
order in which they are written in the code, or
may the compiler reorder them? Explain.
explain:
No, the two 'if' blocks no reorder by compiler. I change the order of two 'if' statements, and increase the for-loop length. and find out, in some cases, the LEDs don't light off, which means after the second if statement, the first 'if' statement does not go to judge. 


7) Calculate the speed of each of the peripheral buses
given the system clock speed and the prescalers
in the `system_stm32f4xx.c` file.

Bus   | Speed (MHz)
----- |------------
AHB   | 168MHz
APB1  | 42MHz
APB2  | 84MHz



8) What line of code in `system_stm32f4xx.c` would you
change to clock the APB1 peripheral bus at 84 MHz? What would you change it to? Try changing this value and run the
`timing-pwm` program again. What is the effect of this change
on the program?
answer: 
this line:
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
which indicate what APB1 bus clock speed. 
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; to adjust the speed to 84MHz. run the program again, the led blink speed increase one time.

9) Write an `initTimer4()` and `initPwm()` function to pulse the green LED every 
20 ms, with a duty cycle of 80%.
answer:
    timerInitStructure.TIM_Period = 80 - 1;
    outputChannelInit.TIM_Pulse = 64;
change these two lines to alter the clock and duty cycle.


10) What is the value of the `lr` at the first line of `TIM2_IRQHandler`? What
does this special value indicate?
answer:
	lr store the special value 0xffffffff. when entering into interrupt, eight registers pushed on stack: R0,R1,R2,R3,R12,LR,PC,PSR. cause there is no parameters and return value for interrupt, so after interrupt, it need to reload the original registers' values. So this lr value indicates to recover these 8 registers from stack after interrupt.

11) Why are hardware-timer-controlled PWM channels more valuable than CPU-controlled PWM 
for outputs with very specific timing requirements? Explain, given what you've observed in your `timing-pwm` experiment.
answer:
	hardware-timer-controlled PWM is controlled by timer4, versus CPU-controlled PWM is by timer2. in timer4 case, we configure timer4 to output PWM at certain time slot. when runs the program, the timer4 output directly drives the GPIOD(LED) bypassing the CPU. While, for timer2, when timer2's interrupt triggered, it firstly send interrupt request to CPU, and CPU looks the vector table to locate the ISR of timer2, and then go to execute ISR. 
	So based on that, we see, if we stop CPU which running at somewhere in the program, we see timer2's interrupt cannot be executed by CPU, however, timer4 not using interrupt continuously blink the led.


