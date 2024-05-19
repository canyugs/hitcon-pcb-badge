/*
 * Uart.h
 *
 *  Created on: May 19, 2024
 *      Author: doraeric
 */

#ifndef HITCON_UART_H_
#define HITCON_UART_H_

#include "usart.h"

namespace hitcon {

const uint8_t RX_BUF_SZ = 64;

void uart_init();
void uart_recv_cb(UART_HandleTypeDef *huart);
void uart_send(const uint8_t *message, uint16_t message_size, uint32_t timeout);

}

#endif /* HITCON_UART_H_ */
