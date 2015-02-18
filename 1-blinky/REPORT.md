Lab 1: Blinky
====================

Lab Assignment | 1 - Blinky
-------------- | -------------
Name           | jizhou
Net ID         | jz1958
Report due     | Tuesday, 17 February


Please answer the following questions:

1) The Makefile for this lab exercise defines, among other things, the compiler to use. (`CC` is a common shorthand used in makefiles for the compiler.) What is the name of the compiler used for this lab exercise?
Answer:
CC=arm-none-eabi-size


2) The Makefile also defines certain *flags* that define compiler behavior. Fill in the following table, matching the flag to the behavior. The first one is completed for you as an example:

Behavior                                  | Flag
----------------------------------------- | -------------
Include debug info for GDB                | `-ggdb`
Compile for a little-endian target        | -mlittle-endian
Use the Thumb instruction set             | -mthumb
Compile for a target with a Cortex M4 CPU | -mcpu=cortex-m4


3) The Makefile also instructs the linker to link files for STM libraries from three locations. What are those locations? (Give the full file path, not the line with variables given in the Makefile.)
Answer:
# Include files from STM libraries
CFLAGS += -I$(STM_COMMON)/Utilities/STM32F4-Discovery
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/Include -I$(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Include
CFLAGS += -I$(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/inc

Location1:/home/jizhou/el6483-jz1958/1-blinky/src/STM32F4-Discovery_FW_V1.1.0/Utilities/STM32F4-Discovery

Location2:/home/jizhou/el6483-jz1958/1-blinky/src/STM32F4-Discovery_FW_V1.1.0/Libraries/CMSIS/Include
and
/home/jizhou/el6483-jz1958/1-blinky/src/STM32F4-Discovery_FW_V1.1.0/Libraries/CMSIS/ST/STM32F4xx/Include

Location3:/home/jizhou/el6483-jz1958/1-blinky/src/STM32F4_Discovery_FW_V1.1.0/Libraries/STM32F4xx_StdPeriph_Driver/inc


4) The Makefile includes information about a startup file, which will be added to the executable file. Startup code is run just after a microcontroller is reset, before the main program. It's usually implemented as universal code for all same microcontroller type, so you won't need to write one from scratch for each program.

What is the location of the startup code? (Give the full file path, not the line with variables given in the Makefile.)
Answer:
location:
/home/jizhou/el6483-jz1958/1-blinky/src/STM32F4-Discovery_FW_V1.1.0/Libraries/CMSIS/ST/STM32F4xx/Source/Templates/TrueSTUDIO

What form is the startup code given in? (C code, assembly, binary?)
Answer:
the startip code is given in assembly code.

5) There are five commands in the .gdbinit file. Look up each command online or in the `gdb` manual. List all five commands here, and explain what each one does.
Answer:
The original .gdbinit file:

#OpenOCD GDB server, enable semihosting, program the flash, and wait for command
target extended localhost:3333
monitor arm semihosting enable
monitor reset halt
load
monitor reset init

Explain:
target type parameters
Conects the GDB host environment to a garget machine or process. A target is typically a protocol for talking to debugging facilities.Using the argument type to specify the type or protocol of the target machine. Further parameters are interpreted by the target protocol, but typically include things like device names or host names to connect with, process numbers, and baud rates.

1. target extended localhost:3333
GDB tries to debug on board, which means need to transfer data between PC and STM32 board. So this command defines #3333 PC port to use to receive and send data to board. Besides the protocol used is extended remote protocol.

2. monitor arm semihosting enable
 semihosting is a mechanism that enables code running on an ARM target to communicate and use the Input/Output facilities on a host computer that is running a debugger. The "monitor" instruction is a prefix command that is used to transfer the remaining command from GDB to OpenOCD using 3333 TCP port. and this means that Openocd will receive the instruction "arm semihosting enable" which enables semihosting.

3. monitor reset halt
reset means unconditionally resets the target system. It is necessary to execute "monitor halt" to regain control of the target after executing this command. "halt" means unconditionally halts the target. All cores on all nodes are halted.

4. load
So far, the OpenOCD which is ready to transmit data. Thus by using load command, which means to download file into board and availabe for debugging on the target by downloading, or dynamic linking.

5. monitor reset init
the optional parameter "init" specifies what should happen after the reset. initlize the buggering process and transfering data to board.


6) What is in each of the ARM registers before execution starts?  Give the output
of `info registers` in the code block below.
Answer: the registers status are showing below.
```
r0             0x0      0
r1             0x0      0
r2             0x0      0
r3             0x0      0
r4             0x0      0
r5             0x0      0
r6             0x0      0
r7             0x0      0
r8             0x0      0
r9             0x0      0
r10            0x0      0
r11            0x0      0
r12            0x0      0
sp             0x0      0x0
lr             0x0      0
pc             0x8001118        0x8001118 <Reset_Handler>
xPSR           0x1000000        16777216


```


7) Copy the assembly code for the `SysTick_Handler` function as shown by `gdb`
into the code block below.


```
   0x080003b0 <+0>:     push    {r7}
   0x080003b2 <+2>:     add     r7, sp, #0
   0x080003b4 <+4>:     ldr     r3, [pc, #16]   ; (0x80003c8 <SysTick_Handler+24>)
   0x080003b6 <+6>:     ldr     r3, [r3, #0]
   0x080003b8 <+8>:     adds    r3, #1
   0x080003ba <+10>:    ldr     r2, [pc, #12]   ; (0x80003c8 <SysTick_Handler+24>)
   0x080003bc <+12>:    str     r3, [r2, #0]
   0x080003be <+14>:    mov     sp, r7
   0x080003c0 <+16>:    ldr.w   r7, [sp], #4
   0x080003c4 <+20>:    bx      lr
   0x080003c6 <+22>:    nop
   0x080003c8 <+24>:    lsrs    r4, r0, #6
   0x080003ca <+26>:    movs    r0, #0

```

8) Include a screenshot showing the "Toggling LED" output in your `openocd` window.
You can use any image hosting service to put your screenshot online.

![](http://url/of/image)
Answer:https://dl-web.dropbox.com/get/toggling%20LED%20state.png?_subject_uid=390030028&w=AABHgRXOtWxVqk-2MFU6IEFiPmAX6yGszCjgapefmkNBuA

9) Include a screenshot showing the "Toggling LED from on to off"
and "Toggling LED from off to on" output in your `openocd` window.
You can use any image hosting service to put your screenshot online.

![](http://url/of/image)
Answer: https://dl-web.dropbox.com/get/onAndOff.png?_subject_uid=390030028&w=AACXjZvqleOZBiPrxYqANZ1LSX5fpXmxMMcSDsTb0s4yBg

10) If you stop `openocd` and `gdb`, and disconnect and reconnect the board from
the host computer, will the program (as given) keep running?
Answer: No, the program is not running
Explain the behavior you observe. What about your modified 'nodebug' program?
Answer: my modified 'nodebug' program could be able to run again. Semihosting is a mechanism that enables code running on an ARM target to communicate and use the Input/Output facilities on a host computer that is running a debugger.
Examples of these facilities include keyboard input, screen output, and disk I/O. For example, you can use this mechanism to enable functions in the C library, such as printf() and scanf(), to use the screen and keyboard of the host instead of having a screen and keyboard on the target system.
So in this case, in modified 'nodebug' file which the functions such as printf() related to the I/O were removed, thus we could not be necessary to run on semihosting mode.

The next step is flashing our program on board. there are two kinds of RAM on board, first one is SRAM which is volatile, second one is flash which is non-volatile which means it could be able to store the data without power. Through USB, out program is actually flashing to the flash, and could be able to store in that.

Thus, when we plug out-and-in, firstly we don't need openocd to interact with board in semihosting mode, second, our program is already stored in the flash. The LED could be able to start to run.
