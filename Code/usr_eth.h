/*
 * eth.h
 *
 *  Created on: Nov 2, 2017
 *      Author: chentuofei
 */

#ifndef ETH_H_
#define ETH_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>


#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/flash.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/lwiplib.h"
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"
#include "drivers/pinout.h"

//*****************************************************************************
//
// Defines for setting up the system clock.
//
//*****************************************************************************
#define SYSTICKHZ               100
#define SYSTICKMS               (1000 / SYSTICKHZ)

//*****************************************************************************
//
// Interrupt priority definitions.  The top 3 bits of these values are
// significant with lower values indicating higher priority interrupts.
//
//*****************************************************************************
#define SYSTICK_INT_PRIORITY    0x80
#define ETHERNET_INT_PRIORITY   0xC0

// power setpoint produced by the optimization run on the backend
extern float pset_1;
extern float pset_2;

struct sensor
{
    float v1;
    float v2;
    float i1;
    float i2;
};

extern uint32_t g_ui32SysClock;

struct tcp_pcb * pcb_glob;

char t_str [50];
// send
char msg_str [50];
// recv

uint32_t seq_num;

uint32_t send_cnt;

err_t err_write;

void SysTickIntHandler(void);

void eth_init(); // initialize the user eth wrappers

err_t eth_send(char* msg, int size); // send

err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err);

int start_application(uint32_t serverIP,uint16_t serverPort);


#endif /* ETH_H_ */
