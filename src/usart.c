/*
 * usart.cpp
 *
 *  Created on: 12.10.2019
 *      Author: Pawel
 */
/*
 * usart.cpp
 *
 *  Created on: 25 lis 2016
 *      Author: Adam Tomaja
 */
#include "stm32l1xx.h"

void send_char(char c) {
	/*
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, c);
	*/
	while(!(USART3->SR & USART_SR_TXE));
	  USART3->DR = c;
}

void print(const char * s) {
	while (*s)
	 send_char(*s++);
}

int usart_receive() {
	if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE)) {
		return USART_ReceiveData(USART3);
	}

	return -1;
}





