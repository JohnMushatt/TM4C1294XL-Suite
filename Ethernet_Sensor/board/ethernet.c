/*
 * ethernet.c
 *
 *  Created on: Jun 27, 2020
 *      Author: johnm
 */

#include "ethernet.h"
#include <string.h>
#include <sys/socket.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
extern int inet_pton(int af, const char *src, void *dst);
#ifndef __EK_TM4C1294XL_H
#include "EK_TM4C1294XL.h"
#endif

struct sockaddr_in serverAddr;
int board_socket;
size_t size;
void tcp_dispatch(void) {

	Task_Handle task_handle;
	Task_Params task_params;
	Error_Block eb;
	Error_init(&eb);
	Task_Params_init(&task_params);
	task_params.stackSize = 1024;
	task_params.priority = 7;
	task_handle = Task_create((Task_FuncPtr) tcp_handler, &task_params, &eb);
	if (task_handle == NULL) {
		System_printf("tcp_dispatch: Failed to create tcp_handler Task\n");
	}
	System_flush();

}
void update_connection(void) {
	con_up = true;
	int status;
	struct sockaddr_in localAddr;
	//struct sockaddr_in serverAddr;

	board_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (board_socket == -1) {
		System_printf("Error: socket not created.\n");
		System_flush();
	}
	memset(&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons(1000);
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	if (inet_pton(AF_INET, "192.168.1.25", &serverAddr.sin_addr) < 0) {
		System_printf("Error: could not find server.\n");
		System_flush();

		return;
	}
	//System_printf("Attempting to connect to %s:%d\n",inet_ntoa(serverAddr.sin_addr),(int) ntohs(serverAddr.sin_port));
	size = sizeof(serverAddr);
	status =connect(board_socket, (struct sockaddr *) &serverAddr,
			sizeof(serverAddr));
	if ( status< 0) {
		System_printf("Error: could not connect to server.\n");
		System_flush();

		return;
	}
}

