/*
 * uart.c
 *
 *  Created on: Sep 26, 2023
 *      Author: HaHuyen
 */
#include "uart.h"

uint8_t receive_buffer1 = 0;
uint8_t index_buffer = 0;
char rcv[100];
uint8_t msg[100];
uint8_t buffer_flag = 0;

void uart_init_rs232(){
	HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
}

void uart_Rs232SendString(uint8_t* str){
	HAL_UART_Transmit(&huart1, (void*)msg, sprintf((void*)msg,"%s",str), 10);
}

void uart_Rs232SendBytes(uint8_t* bytes, uint16_t size){
	HAL_UART_Transmit(&huart1, bytes, size, 10);
}

void uart_Rs232SendNum(uint32_t num){
	if(num == 0){
		uart_Rs232SendString("0");
		return;
	}
    uint8_t num_flag = 0;
    int i;
	if(num < 0) uart_Rs232SendString("-");
    for(i = 10; i > 0; i--)
    {
        if((num / mypow(10, i-1)) != 0)
        {
            num_flag = 1;
            sprintf((void*)msg,"%d",num/mypow(10, i-1));
            uart_Rs232SendString(msg);
        }
        else
        {
            if(num_flag != 0)
            	uart_Rs232SendString("0");
        }
        num %= mypow(10, i-1);
    }
}

void uart_Rs232SendNumPercent(uint32_t num)
{
	sprintf((void*)msg,"%ld",num/100);
    uart_Rs232SendString(msg);
    uart_Rs232SendString(".");
    sprintf((void*)msg,"%ld",num%100);
    uart_Rs232SendString(msg);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	/* Old implementation
	if(huart->Instance == USART1){
		// rs232 isr
		// can be modified
		rcv[index_buffer] = receive_buffer1;
		index_buffer++;
		if (index_buffer == 100) index_buffer = 0;
		buffer_flag = 1;
		HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
	}
	*/

	if (huart->Instance == USART1) {
		// Store the received character into the ring buffer
		rcv[index_buffer] = receive_buffer1;

		// Check for end-of-message (newline character '\n')
		if (receive_buffer1 == '!') {
			rcv[index_buffer] = '\0'; // Null-terminate the message
			buffer_flag = 1;          // Set buffer flag to indicate a complete message
		}

		// Increment the index for the next character
		index_buffer++;

		// Wrap around if the end of the buffer is reached
		if (index_buffer >= 100) {
			index_buffer = 0;
		}

		// Reactivate UART interrupt for the next byte
		HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
	}
}
