/*
 * usr_pwm.h
 *
 *  Created on: Nov 25, 2017
 *      Author: chentuofei
 */

#ifndef USR_PWM_H_
#define USR_PWM_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#include "driverlib/pwm.h"
#include "driverlib/uart.h"

void pwm_duty_set(int chan, int duty);
void pwm_init(int period);
void pwm_enable();
void pwm_disable();

#endif /* USR_PWM_H_ */
