Lab 3: Analog to digital conversion
===================================

Lab Assignment | 3 - Analog to digital conversion
-------------- | -------------
Name           | Jizhou Zhang
Net ID         | jz1958@nyu.edu
Report due     | Monday, 9 March


Please answer the following questions:


Plot the values reported by the temperature sensor when using a 12-bit ADC resolution.
Connect them with a line. 

On the same plot, using a different color, plot the values reported by the 
sensor using a 10-bit resolution. Repeat for 8-bit and 6-bit resolutions, using 
a different color for each.

Put your plot online using any image hosting service and show it here:

![](https://dl-web.dropbox.com/get/lab4_chart.png?_subject_uid=390030028&w=AABGhy8dFFEoV143vKxFCPtHHDDTSwVaaR7Tw0h2arxJDQ)



Comment on the plot (in detail!). What does it show about ADC resolution?
answer:
the line with higher resolution has more accuracy. form the chart, we see that 6-bit resolution has more deviation, the reason is that when doing ADC, only 6 bits digit could be used to represent analog output voltage. 
Let's see 8-bit resolution line, clearly, it's a straight line, although we could have 4 times more accurate comparing with 6-bit resolution, however, the detail information still unveil. this line could not show small jump in measurement. for it is 8 bit, the smallest jump it could measure is 3.3/256=0.0129V. So any jump smaller than this value could not be measured.
Next, 10-bit resolution line, still, it's 4 times more accurate versus 8-bit resolution line, the smallest jump is 3.3/1024=0.0032V. We find some jump in this line.
Finally, 12-bit resolution line, the smallest jump it could measure is 3.3/4096=0.0008V. Also 4 time more accuracy versus 10-bit resolution. We can see this line goes high and low. much approaching to the real analog voltage, in other words, 12-bit resolution reflect more accurate temperature voltage outputed by temperature sensor. 
