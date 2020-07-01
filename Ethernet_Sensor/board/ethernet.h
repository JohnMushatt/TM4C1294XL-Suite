/*
 * ethernet.h
 *
 *  Created on: Jun 27, 2020
 *      Author: johnm
 */

#ifndef BOARD_ETHERNET_H_
#define BOARD_ETHERNET_H_
#include "Utility/kernel_include.h"
#include <stdbool.h>
bool con_up;
void tcp_dispatch(void);
void tcp_handler(UArg arg0, UArg arg11);
void update_connection(void);
#endif /* BOARD_ETHERNET_H_ */
