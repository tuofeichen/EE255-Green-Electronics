/**
 * @file  lab2.c
 * @brief Starter code for lab 2.
 * 
 * @details Modify this file to implement the power meter
 * for lab 2
 * 
 * @author Ned Danyliw
 * @date  09.2015
 */

#include "lab2.h"


// calibration parameters
float volts_per_div = 400.0/4096.0;
float amps_per_div  = 300.0/4096.0;
float power_result;

uint16_t zero_volts;
uint16_t zero_amps;
uint16_t cal_volts;
uint16_t cal_amps;
int    last_voltage_reading;
int last_current_reading;
uint16_t stored_zero_volts;
uint16_t stored_zero_amps;
uint16_t stored_volts_per_div;
uint16_t stored_amps_per_div;
uint16_t shift;

//current readings
__IO uint16_t voltage_reading;
__IO uint16_t current_reading;


/**
 * @brief Calibrate out the 0V and 0A offsets
 * @details Reads the ADC value at 0V and 0A and records
 * their offsets in the EEPROM
 */
void calibrate_offset() {
  zero_volts = voltage_reading;
  zero_amps  = current_reading;

  //Store values in EEPROM
  stored_zero_volts = 0x0000;
  stored_zero_amps  = 0x1000;
  eeprom_write(stored_zero_volts, zero_volts);
  eeprom_write(stored_zero_amps, zero_amps);
}


/**
 * @brief Updates calibration for the standard voltage
 * @details Calculates the calibration value read from the ADC
 * and stores the result in the EEPROM
 */
void calibrate_voltage() {
  //Code to calculate volts_per_div
  char buf [20];
  cal_volts = voltage_reading;
  volts_per_div = CAL_VOLTS / ((float)cal_volts - (float)zero_volts);
  lcd_goto(0, 2);
  snprintf(buf,20,"cal_volts:%d %d",cal_volts, zero_volts);
  lcd_puts(buf);
  //Store values in EEPROM
  stored_volts_per_div = 0x0100;
  eeprom_write(stored_volts_per_div, volts_per_div);
}


/**
 * @brief Updates calibration for the standard current
 * @details Calculates the calibration value read from the ADC
 * and stores the result in the EEPROM
 */
void calibrate_current() {
  //Code to calculate amps_per_div
  char buf[20];
  cal_amps = current_reading;
  amps_per_div = CAL_CURR / (cal_amps - zero_amps);
  lcd_goto(0, 2);
  snprintf(buf,20,"cal_amp:%d %d",cal_amps, zero_volts);
  lcd_puts(buf);
  //Store values in EEPROM
  stored_amps_per_div = 0x0010;
  eeprom_write(stored_amps_per_div, amps_per_div);
}


/**
 * @brief Initialize the energy meter
 * @details Reads the calibration values from the EEPROM
 */
void meter_init() {
  //Read in calibration constants from EEPROM
  eeprom_read(stored_zero_volts, &zero_volts);
  eeprom_read(stored_zero_amps, &zero_amps);
  eeprom_read(stored_volts_per_div, &volts_per_div);
  eeprom_read(stored_amps_per_div, &amps_per_div);
}

/**
 * @brief Displays energy meter data
 * @details Replace with code to update the display with
 * your own
 */
void meter_display() {
  // calculate values
  shift = 2;
  float alpha = 3.0/7.0;
  float beta = 4.0/7.0;
  float volt,amp;
  int v_adc,a_adc;

  v_adc = (int)voltage_reading - zero_volts;

  v_adc = alpha*(float)last_voltage_reading+beta*(float)v_adc; //(last_voltage_reading - (last_voltage_reading >> shift) + voltage_reading) >> shift;
  volt = volts_per_div * v_adc;

  a_adc = current_reading - zero_amps;
  a_adc = alpha*(float)last_current_reading+beta*(float)a_adc;//(last_current_reading - (last_current_reading >> shift) + current_reading) >> shift;
  amp = amps_per_div * a_adc; // gain is 5

  power_result = volt * amp;

  last_voltage_reading = v_adc;
  last_current_reading = a_adc;

  // display values
  size_t size = 64;
  char buff[size];
  snprintf(buff, size, "voltage: %4.2f V", volt);

  lcd_clear();
  lcd_goto(0, 0);
  lcd_puts(buff);

  snprintf(buff, size, "Ampere: %4.2f A", amp);;
  lcd_goto(0, 1);
  lcd_puts(buff);

  snprintf(buff, size, "Power: %4.2f W", power_result);;
  lcd_goto(0, 2);
  lcd_puts(buff);

}


/**
 * @brief Callback at end of ADC conversion
 * @details Called at the end of the ADC conversions
 */
void my_adc_callback(uint16_t *data) {
  voltage_reading = (uint16_t) data[0];
  current_reading = (uint16_t) data[1];
}
