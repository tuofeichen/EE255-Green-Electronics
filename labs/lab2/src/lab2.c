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
float volts_per_div;
float amps_per_div;
uint16_t zero_volts;
uint16_t zero_amps;

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
  zero_amps = current_reading;

  //Store values in EEPROM

}


/**
 * @brief Updates calibration for the standard voltage
 * @details Calculates the calibration value read from the ADC
 * and stores the result in the EEPROM
 */
void calibrate_voltage() {
  //Code to calculate volts_per_div

  //Store values in EEPROM
}


/**
 * @brief Updates calibration for the standard current
 * @details Calculates the calibration value read from the ADC
 * and stores the result in the EEPROM
 */
void calibrate_current() {
  //Code to calculate amps_per_div

  //Store values in EEPROM
}


/**
 * @brief Initialize the energy meter
 * @details Reads the calibration values from the EEPROM
 */
void meter_init() {
  //Read in calibration constants from EEPROM

  
}


/**
 * @brief Displays energy meter data
 * @details Replace with code to update the display with
 * your own
 */
void meter_display() {
  char buf1[20];
  char buf2 [20];

  snprintf(buf1,20,"Test Volt: %d",voltage_reading);
  // sprintf(buf,"V:%4.2f",(float)voltage_reading*volts_per_div);
  lcd_clear();
  lcd_goto(0,0);
  lcd_puts(buf1);

  snprintf(buf2,20,"Test Current:%d",current_reading);
  lcd_goto(0,1);
  lcd_puts(buf2);
  //Code here
}


/**
 * @brief Callback at end of ADC conversion
 * @details Called at the end of the ADC conversions
 */


void my_adc_callback(uint16_t *data) {

  voltage_reading = (uint16_t) data[0];
  current_reading = (uint16_t) data[1];

}
