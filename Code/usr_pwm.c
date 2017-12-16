/*
 * usr_pwm.c
 *
 *  Created on: Nov 25, 2017
 *      Author: chentuofei
 */

#include "usr_pwm.h"

void pwm_init(int period)
{
   SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0))
    {

    }

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinConfigure(GPIO_PF2_M0PWM2);
    GPIOPinConfigure(GPIO_PF3_M0PWM3);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    GPIOPinConfigure(GPIO_PK4_M0PWM6);
    GPIOPinConfigure(GPIO_PK5_M0PWM7);

    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinTypePWM(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5);

    PWMClockSet(PWM0_BASE,PWM_SYSCLK_DIV_2); // clock divider setting

//    appliance 1
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
//    appliance 2
    PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, period);

    PWMDeadBandEnable(PWM0_BASE,PWM_GEN_1,30,30);

    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, period);

    PWMDeadBandEnable(PWM0_BASE,PWM_GEN_3,30,30);
}

void pwm_duty_set(int chan, int duty)
{
    if (chan == 1 )
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, duty);
    else if (chan==2)
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, duty);
    else
        UARTprintf("Invalid Channel!\n");

}

void pwm_enable()
{
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);
    PWMOutputState(PWM0_BASE, (PWM_OUT_2_BIT | PWM_OUT_3_BIT), true);

    PWMGenEnable(PWM0_BASE, PWM_GEN_3);
    PWMOutputState(PWM0_BASE, (PWM_OUT_6_BIT | PWM_OUT_7_BIT), true);
}

void pwm_disable()
{
    PWMGenDisable(PWM0_BASE, PWM_GEN_1);
    PWMOutputState(PWM0_BASE, (PWM_OUT_2_BIT | PWM_OUT_3_BIT), false);

    PWMGenDisable(PWM0_BASE, PWM_GEN_3);
    PWMOutputState(PWM0_BASE, (PWM_OUT_6_BIT | PWM_OUT_7_BIT), false);
}
