Lab 6: RTOS
====================

Lab Assignment | 6 - RTOS
-------------- | -------------
Name           | jizhou
Net ID         | jz1958
Report due     | Thursday, 23 April


Please answer the following questions:

1) In the previous lab, we learned about debouncing inputs that may "bounce,"
such as buttons. In this lab, we light LEDs according to the status of a button input. 
Do we need to debounce the button input in this lab? Why or why not? If your
answer is "yes," explain what user-perceivable problem you would solve
by debouncing button input in this lab. If your answer is "no," explain what's different
about this lab and the previous lab, in which we demonstrated user-perceivable problems
due to failure to debounce button input.
Answer: no need to do debounce in this lab. Comparing with the previous lab, this lab does not necessary to judge the times of press button. However, in the previous lab, we need to more precisely judge the locations. In other word, for this lab, the detail glitch would not influence the consequence.


2) Show the backtrace of the main thread while inside the `Blinky()` function, 
using thread debugging in `gdb`.
answer: ![backtrace](https://dl-web.dropbox.com/get/lab6_backtrace.png?_subject_uid=390030028&w=AABegPZrDa_Xj8f4iM9qxFwyoKn0dY5cu4fwFoEqvYvHjw)


3) In lab 2, I asked you to check the size of your ELF file
in two cases: one without the peripheral library and one with the peripheral
library. What is the size of your ELF file for the RTOS? Use `size build/rtos-blinky.elf` 
to get its size, then use your work from lab 2 to fill out the following table:

Implementation                    | ELF size (hex)
--------------------------------- | -------------
Lab 2 - GPIO with no libraries    | 90c
Lab 2 - with peripheral libraries | 182c
Lab 6 - with RTOS                 | 193c



4) The ChibiOS build system lets us enable and disable parts of the OS and
device drivers on an individual basis. Of course, there are dependencies 
between the OS kernel and device subsystems. As an example, try enabling 
PWM in `halconf.h` in the `rtos-blinky` project. Then try to build the project with `make`. 
What error message do you get? Show this error message here. Then,
identify the line in `mcuconf.h` that you need to change in order 
to fix this error and successfully build the OS with PWM support. Explain.
Answer: 
first, modify the line in halconf.h to the following:
#define HAL_USE_PWM                 TRUE
then, compiled the project, and got the following error message:
![errorMessage](https://dl-web.dropbox.com/get/lab6_error.png?_subject_uid=390030028&w=AACs8OXYf__tgLi7dBUOXskKdlsL-8HSLL829hWkTDwYuw)
in order to fix this error, changing the line to the following:
#define STM32_PWM_USE_TIM1                  TRUE
after that, the compile could successfully pass.


5) List the commands available in the ChibiOS shell in `rtos-shell`, and describe
what each one appears to do.
answer:
help, show all the commands that can be used in shell
exix, print the ChibiOS shell quit
info, print the information about OS kernel, compiler, architecture, board information
systime, show the current system time
mem, show the memory occupation information
threads, list all the threads that run on board.

6) Show the output of the `priority` command with the letters A-E in the correct order:

```
  ![priority](https://dl-web.dropbox.com/get/lab6_priority.png?_subject_uid=390030028&w=AAAYlYdN8T4B24vtNznMQ6GfZi-M7QEvJjlSjOCzysI0tA)
```

Also show the modifications you made to these five lines of code:

```
  Thread *tp1 = chThdCreateFromHeap(NULL, WA_SIZE, NORMALPRIO-10, thread1, chp);
  Thread *tp2 = chThdCreateFromHeap(NULL, WA_SIZE, NORMALPRIO-8, thread2, chp);
  Thread *tp3 = chThdCreateFromHeap(NULL, WA_SIZE, NORMALPRIO, thread3, chp);
  Thread *tp4 = chThdCreateFromHeap(NULL, WA_SIZE, NORMALPRIO-1, thread4, chp);
  Thread *tp5 = chThdCreateFromHeap(NULL, WA_SIZE, NORMALPRIO-5, thread5, chp);
```

7) Compute the average waiting time and average turnaround time for the tasks 
in the `rtos-round-robin` example.  
(See [Homework 8](http://witestlab.poly.edu/~ffund/el6483/files/homework8.pdf).) 
You can neglect CPU time spent on tasks other than the 5 "test" tasks created for the example.
answer:
Waiting Time = (Final Start Time - Previous Time in CPU - Arrival Time)
Turnaround Time = (End Time - Arrival Time)
And because it uses round-robin algorithm, so thread 1 to thread 5 will be executed in turn. 
Waiting Time:
thread1: 751+(1502-751)+(2253-1502) = 2253
thread2: 2253
thread3: 2253
thread4: 2252
thread5: 2252
Turnaround Time:
thread1: 2253
thread2: 2253
thread3: 2253
thread4: 2252
thread5: 2252
Average Waiting Time:
2253 Tick
Average Turnaround Time:
2253 Tick


8) What is the output of the `roundrobin` command in the `rtos-round-robin` example
when you modify thread 3 to have a higher priority than the other threads?

```
thread3   64    0
thread3   64    751
thread3   64    1501
thread3   64    2252
thread1   62    0
thread2   62    0
thread4   62    0
thread5   62    0
thread1   62    751
thread2   62    750
thread4   62    751
thread5   62    751
thread1   62    1502
thread2   62    1502
thread4   62    1502
thread5   62    1502
thread1   62    2253
thread2   62    2253
thread4   62    2253
thread5   62    2253
```

Draw a chart (like the one on page 40 of the lecture slides) showing the scheduling
of these five tasks using the round robin scheduler with priority. Note the
priority of each task on the chart. Upload it to any image hosting service and
show the image inline here:

![draw](https://dl-web.dropbox.com/get/lab6_draw.png?_subject_uid=390030028&w=AABvtEZnN1YGsHvxWeInhwua3vWSViEVbkR-D-krQoQoiA)

9) What is the computation time and period of each of the tasks in the `rtos-rms` 
project? Fill in the following table.

Task          | Computation time (system ticks) | Period (system ticks)
------------- | --------------------------------|-----------------------
t1            |               72                | 300
t2            |               38                | 170
t3            |               57                | 280
t4            |               24                | 380

What is the CPU utilization? Is there a guaranteed RMS schedule that 
will meet the tasks' deadlines?
72/300+38/170+57/280+24/380 = 73%
n*(2^(1/n)-1) = 75%
So RMS schedule will meet the tasks' deadlines.



10) What priority did you assign to each task for rate monotonic scheduling?


Task          | Priority
------------- | --------------
t1            |  -2             
t2            |  -0           
t3            |  -1         
t4            |  -3         

