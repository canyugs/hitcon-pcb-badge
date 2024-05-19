/*
 * Uart.cpp
 *
 *  Created on: May 19, 2024
 *      Author: doraeric
 */

#include "uart.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	hitcon::uart_recv_cb(huart);
}

namespace hitcon {

static int rx_idx = 0;
uint8_t tx_buf[] = "Hello World";
uint8_t rx_buf[RX_BUF_SZ] = {0};

void uart_init() {
	uart_recv_cb(&huart2);
}

void uart_recv_cb(UART_HandleTypeDef *huart) {
	HAL_UART_Receive_IT(&huart2, (uint8_t *)(rx_buf+(rx_idx % RX_BUF_SZ)), 1);
	++rx_idx;
}
void uart_send(const uint8_t *message, uint16_t message_size, uint32_t timeout) {
	HAL_UART_Transmit(&huart2, message, message_size, timeout);
}

} /* namespace hitcon */
