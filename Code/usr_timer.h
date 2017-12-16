/*
 * usr_timer.h
 *
 *  Created on: Nov 26, 2017
 *      Author: chentuofei
 */

#ifndef USR_TIMER_H_
#define USR_TIMER_H_

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "drivers/pinout.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

void timer_init(uint32_t freq);
void Timer0BIntHandler(void);
void swap(int* a, int* b);
int partition (int arr[], int low, int high);

void quickSort(int arr[], int low, int high);

int getMedian(int* arr);


extern bool int_flag;
extern int  int_cnt;

#endif /* USR_TIMER_H_ */
