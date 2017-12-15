/*
 * usr_adc.c
 *
 *  Created on: Nov 25, 2017
 *      Author: chentuofei
 */

#include "usr_adc.h"

int v_sense_main = 0.0;
int v_sense_app1 = 0.0;
int v_sense_app2 = 0.0;
int i_sense_main = 0.0;
int i_sense_app1 = 0.0;
int i_sense_app2 = 0.0;

int v_max_main = 0.0;
int v_max_app1 = 0.0;
int v_max_app2 = 0.0;
int i_max_main = 0.0;
int i_max_app1 = 0.0;
int i_max_app2 = 0.0;

int v_main_offset = 0;
int v_app1_offset = 0;
int v_app2_offset = 0;

float pwr_main = 0.0;
float pwr_app1 = 0.0;
float pwr_app2 = 0.0;

void adc_init(int seq)
{

    //
    // Enable the ADC0 module.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

//    all the pins we need
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_7);
    GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_4);

    // Wait for the ADC0 module to be ready.
    //
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
    {

    }

    ADCSequenceConfigure(ADC0_BASE, seq, ADC_TRIGGER_PROCESSOR, 0); // run by user command
//    ADCSequenceConfigure(ADC0_BASE, seq, ADC_TRIGGER_TIMER, 0); // need to test out

//    first read channel 2. this should be unnecessary as we're overwriting it
    ADCSequenceStepConfigure(ADC0_BASE, seq, 0,
    ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH2);

    ADCSequenceEnable(ADC0_BASE, seq);

    adc_calibrate(100);

}

void adc_calibrate(int num)
{
    int ii = 0;
    char buf[50];

    while (ii++ < num)
    {
        v_main_offset += adc_read(3, V_MAIN_PIN);
        v_app1_offset += adc_read(3, V_APP1_PIN);
        v_app2_offset += adc_read(3, V_APP2_PIN);
    }

    v_main_offset /= num;
    v_app1_offset /= num;
    v_app2_offset /= num;


    usprintf(buf, "ADC: V_cali (main,v1,v2) = %d, %d, %d \n", v_main_offset,v_app1_offset,v_app2_offset);
    UARTprintf("%s", buf);

}

// ADC_CTL_CHx manual read (can migrate to interupt read later)
int adc_read(int seq, int chan)
{

    uint32_t ui32adc0val;
    int time_average = 0;

    ADCSequenceStepConfigure(ADC0_BASE, seq, 0,
    ADC_CTL_IE | ADC_CTL_END | chan);

    ADCProcessorTrigger(ADC0_BASE, seq);
    //
    // Wait until the sample sequence has completed.
    //
    while (!ADCIntStatus(ADC0_BASE, seq, false))
    {

    }

    // Clear the ADC interrupt flag.
    //
    ADCIntClear(ADC0_BASE, seq);

    // Read the value from the ADC.
    //
    int i = 0;
    for (i = 0; i < 5; i++)
    {
       ADCSequenceDataGet(ADC0_BASE, seq, &ui32adc0val);
       time_average+=ui32adc0val;

    }

    // average the sensors input
    time_average/=5;

    return ui32adc0val;
}
