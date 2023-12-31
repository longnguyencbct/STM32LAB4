/*
 * fsm_uart.c
 *
 *  Created on: Nov 7, 2023
 *      Author: clong
 */

#include "fsm_uart.h"
#include "main.h"
#include <inttypes.h>
#include "software_timer.h"
enum CMD_STATE {
		CMD_INIT,
		CMD_BODY
};
enum UART_STATE {
		UART_INIT,
		UART_WAIT
};

static enum CMD_STATE cmd_state = CMD_INIT;// current state of command_parser_fsm
static enum UART_STATE uart_state = UART_INIT;//current state of uart_communiation_fsm


uint8_t curr_idx_buffer = 0;// index of buffer
uint8_t cmd_flag = 0;// 1=Got a command, 0=Nothing
char cmd_data [MAX_BUFFER_SIZE]= "";//storing command, exclude "!" and "#" symbols
uint8_t idx_cmd_data=0;//current index of char cmd_data

uint8_t strr[30];//array to store output string
uint32_t ADC_value;

void command_parser_fsm (){
	switch (cmd_state){
	case CMD_INIT: // check for "!" from input to switch to CMD_BODY
		if(buffer[curr_idx_buffer]=='!') {
			curr_idx_buffer++;
			idx_cmd_data = 0;
			cmd_flag = 0;
			cmd_state = CMD_BODY;
		}
		break;
	case CMD_BODY://scan for a command or a string to store
		if(buffer[curr_idx_buffer]=='!') break;
		if(buffer[curr_idx_buffer]=='#') {
			cmd_data[idx_cmd_data] = '\0';
			if(strcmp(cmd_data, "RST") == 0) {
				cmd_flag = 1;
			}
			else if(strcmp(cmd_data,"OK")  == 0) {
				cmd_flag = 1;
			}
			cmd_state = CMD_INIT;
		}
		else {
			cmd_data[idx_cmd_data++] = buffer[curr_idx_buffer];
		}
		curr_idx_buffer++;
		if(curr_idx_buffer==30) curr_idx_buffer=0;
		break;
	}
}
void uart_communiation_fsm () {
	switch (uart_state){
	case UART_INIT:
		if(cmd_flag==1 && strcmp(cmd_data, "RST") == 0) {
			cmd_flag = 0;
			uart_state = UART_WAIT;
			HAL_GPIO_TogglePin (LED_YELLOW_GPIO_Port, LED_YELLOW_Pin ) ;
			ADC_value = HAL_ADC_GetValue (&hadc1);
			int len = sprintf ( strr , "\r\n!ADC=%d#\r\n", ADC_value );
			HAL_UART_Transmit (&huart2 , (void *) strr, len, 1000);
			setTimer(0,300);
		}
		break;
	case UART_WAIT:
		if(cmd_flag==1 && strcmp(cmd_data, "OK") == 0) {
			cmd_flag = 0;
			uart_state = UART_INIT;
		}
		else if (timer_flag[0]==1){
			int len = sprintf ( strr , "\r\n!ADC=%d#\r\n", ADC_value );
			HAL_UART_Transmit (&huart2 , (void *) strr, len, 1000);
			setTimer(0,300);
		}
		break;
	}
}
