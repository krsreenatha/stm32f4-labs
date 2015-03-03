Lab 2: GPIO Peripherals
====================

Lab Assignment | 2 - GPIO Peripherals
-------------- | -------------
Name           | jizhou
Net ID         | jz1958
Report due     | Tuesday, 3 March


Please answer the following questions:


1) What is the size of your ELF file for the first part of the lab exercise, 
in which you write to GPIO registers directly? Show the output 
of `size build/button-gpio.elf` here.
Answer:
   size build/button-gpio.elf
   text	   data	    bss	    dec	    hex	filename
   1272	     20	   1024	   2316	    90c	build/button-gpio.elf



2) What is the size of your ELF file for the second part of the lab exercise, 
in which you use GPIO libraries? Show the output 
of `size build/button-library.elf` here. 
Answer:
   size build/button-library.elf
   text	   data	    bss	    dec	    hex	filename
   5128	     36	   1024	   6188	   182c	build/button-library.elf



3) Compare your answers to 1) and 2). Is this consistent with your expectations? Explain.
Answer:
Yes, it is consistent with my expectations. for the second would pre-compile not only the necessary libraries such as stm32f4xx.c but also the the libraries include functions to initilize and control GPIO and RCC. So these libraries occupy the memory and the second .elf file's larger than the first .elf file.



4) Using `x`, show the contents of the input data register for the GPIO A port when the 
user button is pressed *and* when it is not pressed. 

The Markdown syntax for an image is included below for your convenience; upload your screenshot 
to any image hosting service and then replace the URL for the *image file* in the 
line below. 

![](https://dl-web.dropbox.com/get/lab3_IDRvalue.png?_subject_uid=390030028&w=AAAfTsz0gaE9xrrotJW8Xb1P8OdbBayFfxuywz0ZkjCPkQ)


5) Which bit in this value changes when the button is pressed? Why?
Answer:
The LSB is changed when the button is pressed. because check the bit configuration in GPIOA->IDR, the bit 0 indicate whether any input to the register, and user_button is connected to this bit. So its value would represent if the button is pressed or not. if pressend the voltage is Vdd, which is 1 in binary number; otherwise, the value is ground voltage which is 0.

