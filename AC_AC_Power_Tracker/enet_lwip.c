//*****************************************************************************
// AC-AC Converter for Green Electronics Final Project
// PWM power control
//*****************************************************************************

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "drivers/pinout.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

#include "utils/uartstdio.h"
#include "utils/lwiplib.h"

// defined library
#include "usr_eth.h"
#include "usr_pwm.h"
#include "usr_adc.h"
#include "usr_timer.h"

//*****************************************************************************
//
// The current IP address.
//
//*****************************************************************************
uint32_t g_ui32IPAddress;

//*****************************************************************************
//
// System clock rate in Hz.
//
//*****************************************************************************
uint32_t g_ui32SysClock;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}

//*****************************************************************************
//
// Display an lwIP type IP Address.
//
//*****************************************************************************
void DisplayIPAddress(uint32_t ui32Addr)
{
    char pcBuf[16];

    //
    // Convert the IP Address into a string.
    //
    usprintf(pcBuf, "%d.%d.%d.%d\n", ui32Addr & 0xff, (ui32Addr >> 8) & 0xff,
             (ui32Addr >> 16) & 0xff, (ui32Addr >> 24) & 0xff);

    //
    // Display the string.
    //
    UARTprintf(pcBuf);
}

//*****************************************************************************
//
// Required by lwIP library to support any host-related timer functions.
//
//*****************************************************************************
void lwIPHostTimerHandler(void)
{
    uint32_t ui32Idx, ui32NewIPAddress;

    //
    // Get the current IP address.
    //
    ui32NewIPAddress = lwIPLocalIPAddrGet();

    //
    // See if the IP address has changed.
    //
    if (ui32NewIPAddress != g_ui32IPAddress)
    {
        //
        // See if there is an IP address assigned.
        //
        if (ui32NewIPAddress == 0xffffffff)
        {
            //
            // Indicate that there is no link.
            //
            UARTprintf("Waiting for link.\n");
        }
        else if (ui32NewIPAddress == 0)
        {
            //
            // There is no IP address, so indicate that the DHCP process is
            // running.
            //
            UARTprintf("Waiting for IP address.\n");
        }
        else
        {
            //
            // Display the new IP address.
            //
            UARTprintf("Device IP Address: ");
            DisplayIPAddress(ui32NewIPAddress);
        }

        //
        // Save the new IP address.
        //
        g_ui32IPAddress = ui32NewIPAddress;

        //
        // Turn GPIO off.
        //
        MAP_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, ~GPIO_PIN_1);
    }

    //
    // If there is not an IP address.
    //
    if ((ui32NewIPAddress == 0) || (ui32NewIPAddress == 0xffffffff))
    {
        //
        // Loop through the LED animation.
        //

        for (ui32Idx = 1; ui32Idx < 17; ui32Idx++)
        {

            //
            // Toggle the GPIO
            //
            MAP_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1,
                             (MAP_GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1) ^
                             GPIO_PIN_1));

            SysCtlDelay(g_ui32SysClock / (ui32Idx << 1));
        }
    }
}

//*****************************************************************************
//
// The interrupt handler for the SysTick interrupt.
//
//*****************************************************************************

int main(void)
{
    SysCtlMOSCConfigSet(SYSCTL_MOSC_HIGHFREQ);
    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
    SYSCTL_OSC_MAIN |
    SYSCTL_USE_PLL |
    SYSCTL_CFG_VCO_480),
                                            120000000);

    uint32_t adc_val = 0;

    // Configure the device pins.
    //
    PinoutSet(true, false);

    // Configure indicator LED
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    // Configure two toggle pins for timing debug
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);


    // Initialize the UART.
    ConfigureUART();

    // Start Setting up
    UARTprintf("\033[2J\033[H");
    UARTprintf("System Setup ...\n");
    UARTprintf("=================\n\n");

    //set up ethernet
    eth_init();

    // setup pwm
    uint32_t period = 1000;
    pwm_init(period);
    pwm_duty_set(1, 1);//period);
    pwm_duty_set(2, 1);//period);

    pwm_enable();
    UARTprintf("PWM: Clock freq %d,period %d\n", g_ui32SysClock, period);
//
    // setup adc
    int seq = 3;
    adc_init(seq);

    // setup 6khz timer interrupt for ADC processing (sample 100 points every AC cycle)
    timer_init(g_ui32SysClock / 6000);

    // delay a bit for ethernet set up
    SysCtlDelay(g_ui32SysClock / 10);

    UARTprintf("\nFinished Setting Up enter main loop \n\n", g_ui32SysClock,
               period);

    int cnt = 0;   // refresh cnt
    char buf[50];  // print buffer

    float kp = 0.3;
    float kd = 0.001; // start with small kd

    float err_prev_1 = 0.01;
    float err_1 = 0.0;

    float err_prev_2 = 0.01;
    float err_2 = 0.0;

    int pwm_1 = 0; // from 0-1000
    int pwm_2 = 0; // from 0-1000

    bool blink = 0;
    int but = 0;
    int but_prev = 3;

    while (1)
    {

        if (int_flag)
        {

            but = GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0|GPIO_PIN_1);
            int_flag = 0; // clear interrupt flag, do one iteration of PID

            err_1 = pset_1 - pwr_app1;
            err_2 = pset_2 - pwr_app2;

//            pwm_1 += kp * err_1 + (err_1 - err_prev_1) * kd;
//            pwm_2 += kp * err_2 + (err_2 - err_prev_2) * kd;

            err_prev_1 = err_1;
            err_prev_2 = err_2;

//            debug pwm change from user input
            if ((but_prev ==3)&&(but!=3))
            {
                if (but == 1)
                {
                    pwm_1 += 50;
                    pwm_2 += 50;
                }
                else if (but == 2)
                {
                    pwm_1 -= 50;
                    pwm_2 -= 50;
                }
            }
            but_prev = but;

//        saturation at PWM 1 (0 will not work!)
            if (pwm_1 > period)
                pwm_1 = period;
            else if (pwm_1 < 1)
                pwm_1 = 1;

            if (pwm_2 > period)
                pwm_2 = period;
            else if (pwm_2 < 1)
                pwm_2 = 1;

//          threshold on pset 1 (avoiding high voltage switching?)

            //
//            pwm_duty_set(1, pwm_1);
            pwm_duty_set(2, pwm_2);
        }

        if (!(cnt++ % 2000000))
        {

            sprintf(buf, "Power (main,1,2)=%4.2f, %4.2f, %4.2f\n", pwr_main,
                    pwr_app1, pwr_app2);
//            sprintf(buf,"Pset 1 %4.2f PSet 2 %4.2f\n",pset_1,pset_2);
            UARTprintf("%s", buf);
            sprintf(buf,"pwm_1 %d pwm_2:%d\n\n",pwm_1,pwm_2);
            UARTprintf("%s", buf);
//        sprintf(buf, "I_sense (main,v1,v2) = %4.2f, %4.2f, %4.2f \n", i_sense_main,i_sense_app1,i_sense_app2);
//        UARTprintf("%s", buf);
        }
    }
}

