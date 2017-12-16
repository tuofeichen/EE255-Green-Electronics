/*
 * usr_adc.h
 *
 *  Created on: Nov 25, 2017
 *      Author: chentuofei
 */

#ifndef USR_ADC_H_
#define USR_ADC_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"

#define I_MAIN_PIN ADC_CTL_CH2
#define I_APP1_PIN ADC_CTL_CH3
#define I_APP2_PIN ADC_CTL_CH8 // this originally connected to PA6

#define V_MAIN_PIN ADC_CTL_CH0
#define V_APP1_PIN ADC_CTL_CH4
#define V_APP2_PIN ADC_CTL_CH1

#define TEMP_SENS_PIN ADC_CTL_CH10



// sequencer to be used for initialization
void adc_init(int seq);
int adc_read(int seq,int chan);
void adc_calibrate(int num);

// sensor readings
extern int v_sense_main;
extern int v_sense_app1;
extern int v_sense_app2;
extern int i_sense_main;
extern int i_sense_app1;
extern int i_sense_app2;

extern int v_main_offset;
extern int v_app1_offset;
extern int v_app2_offset;


// max values of each cycle to compute the rms
extern int v_max_main;
extern int v_max_app1;
extern int v_max_app2;
extern int i_max_main;
extern int i_max_app1;
extern int i_max_app2;

extern float pwr_main;
extern float pwr_app1;
extern float pwr_app2;

#endif /* USR_ADC_H_ */
