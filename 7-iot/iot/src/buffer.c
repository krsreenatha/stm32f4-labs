#include <stdint.h>
#include "buffer.h"
#include <stdio.h>

/**
 * @brief Initialize buffer, set write and read index to zero
 * 
 */
void initBuffer(buffer* buffer){
	int i;
	buffer->writeIndex = 0;
	buffer->readIndex = 0;
	for(i=0;i<BUFF_SIZE;i++){
		buffer->buff[i]=0;
	}
}


/**
 * @brief Write to circular buffer, update write index.
 * 
 * This function does *not* check for overflow
 * 
 */
void write(buffer* buffer, uint8_t value){
	buffer->buff[buffer->writeIndex]=value;
	buffer->writeIndex++;
	if(buffer->writeIndex==BUFF_SIZE) {
		buffer->writeIndex=0;
	}
}


/**
 * @brief Read oldest byte in circular buffer, update read index. 
 * 
 * This function does *not* check for overflow/underflow
 * 
 */
volatile uint8_t read(buffer* buffer){
	uint8_t temp;
	temp=buffer->buff[buffer->readIndex];
	buffer->readIndex++;
	if(buffer->readIndex==BUFF_SIZE){
		buffer->readIndex=0;
	}
	return temp;
}


/**
 * @brief Return size of unread data in circular buffer. 
 * 
 * This function does *not* check for overflow/underflow
 * 
 */
volatile uint8_t size(buffer* buffer){
	return ((uint8_t)(buffer->writeIndex - buffer->readIndex) % BUFF_SIZE);
}

/*
 * the functions to test the Buffer module
 * this function first initialize a buffer with the size of 8
 * then input several data into the buffer and read them out,
 * at the meantime, print the size of buffer.
 */
void testbuffer(){ 
	uint8_t data[]={10,11,12,13,14,15,16,17};
	uint8_t readdata;
	uint8_t sizebuf;
	buffer buffer;
	initBuffer(&buffer);
	//write(&buffer, data[0]);
	//readdata = read(&buffer);
	printf("%d\n",readdata);
	for(int i=0;i<8;i++){
		write(&buffer,data[i]);
		readdata=buffer.writeIndex;
		printf("\nthe write index is: %d", readdata);
	}
	for(int i=0;i<8;i++){
		readdata=read(&buffer);
		printf("\nthe read value is: %d",readdata);
		readdata = buffer.readIndex;
		printf("\nthe read index is: %d", readdata);
		sizebuf = size(&buffer);
		printf("\nsize of buffer is: %d",sizebuf);
	}
}


