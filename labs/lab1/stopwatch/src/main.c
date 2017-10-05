/**
 * @file  main.c
 * @brief Hello World demo for the Green Electronics libraries.
 * 
 * @details Demos the basic functionality of the Green Electronics
 * libraries.
 * 
 * @author Ned Danyliw
 * @date  09.2016
 */
#include "ge_libs.h"
#include <stdio.h>

__IO bool led_state;

bool led_speed;
timer_id_t led_timer;

timer_id_t watch_timer;
int disp_time = 0;
bool stop = 0;

// keep track of what state the board is in
uint8_t ui_state;
// UI state variable
int num_refresh;

#define NUM_STATES 4
// enumerate states
enum UI_STATE {
  LED_DEMO, PWM_DEMO, ADC_DEMO, USART_DEMO
};

// ADC variables
#define NUM_ADC 4
// variable to hold results
__IO uint16_t val[NUM_ADC];
// ADC channels to convert
ADC_CHAN_Type chan_to_conv[NUM_ADC] = {
  GE_ADC1_2, GE_ADC1_3, GE_ADC1_4, GE_ADC2_1
};


void setup_led_gpio() {
  //Initialize LED pins and set as outputs
  gpio_setup_pin(DISC_LD3, GPIO_OUTPUT, false, false);
  gpio_setup_pin(DISC_LD4, GPIO_OUTPUT, false, false);
  gpio_setup_pin(DISC_LD5, GPIO_OUTPUT, false, false);
  gpio_setup_pin(DISC_LD6, GPIO_OUTPUT, false, false);
  gpio_setup_pin(DISC_LD7, GPIO_OUTPUT, false, false);
  gpio_setup_pin(DISC_LD8, GPIO_OUTPUT, false, false);
  gpio_setup_pin(DISC_LD9, GPIO_OUTPUT, false, false);
  gpio_setup_pin(DISC_LD10, GPIO_OUTPUT, false, false);
}

void led_off() {
  gpio_write_pin(DISC_LD3, GPIO_LOW);
  gpio_write_pin(DISC_LD4, GPIO_LOW);
  gpio_write_pin(DISC_LD5, GPIO_LOW);
  gpio_write_pin(DISC_LD6, GPIO_LOW);
  gpio_write_pin(DISC_LD7, GPIO_LOW);
  gpio_write_pin(DISC_LD8, GPIO_LOW);
  gpio_write_pin(DISC_LD9, GPIO_LOW);
  gpio_write_pin(DISC_LD10, GPIO_LOW);
}

void led_on() {
  gpio_write_pin(DISC_LD3, GPIO_HIGH);
  gpio_write_pin(DISC_LD4, GPIO_HIGH);
  gpio_write_pin(DISC_LD5, GPIO_HIGH);
  gpio_write_pin(DISC_LD6, GPIO_HIGH);
  gpio_write_pin(DISC_LD7, GPIO_HIGH);
  gpio_write_pin(DISC_LD8, GPIO_HIGH);
  gpio_write_pin(DISC_LD9, GPIO_HIGH);
  gpio_write_pin(DISC_LD10, GPIO_HIGH);
}

// method to toggle LEDs
void toggle_led() {
    // lcd_clear();
    // lcd_goto(0,0);
    // lcd_puts("asdf");
  if (led_state) {
    led_off();
    led_state = false;
  } else {
    led_on();
    led_state = true;
  }
}

void update_time()
{
  if (!stop)
    disp_time++;
}

// handle initilization code for changing states
void change_state() {
  switch (ui_state) {
    case LED_DEMO:
      setup_led_gpio();
      led_timer = timer_register(500, &toggle_led, GE_PERIODIC);
      timer_start(led_timer);
      lcd_clear();
      lcd_goto(0, 0);
      lcd_puts("LED Demo");

      printf("LED Demo\n");
      break;
    case PWM_DEMO:
      // enable pwm pins
      pwm_set_pin(PE9);
      pwm_set_pin(PE11);
      pwm_set_pin(PE13);
      pwm_set_pin(PE14);

      lcd_clear();
      lcd_goto(0, 0);
      lcd_puts("PWM Demo");

      printf("PWM Demo\n");
      break;
    case ADC_DEMO:
      lcd_clear();
      lcd_goto(0, 0);
      lcd_puts("ADC Demo");

      printf("ADC Demo\n");
      break;
    case USART_DEMO:
      lcd_clear();
      lcd_goto(0, 0);
      lcd_puts("USART Demo");
      lcd_goto(0, 1);

      printf("USART Demo. Echo user input.\n");
      break;
    default:
      break;
  }
}

// ADC callback
void my_adc_callback(uint16_t *buf) {
  for (int k = 0; k < NUM_ADC; k++) {
    val[k] = buf[k];
  }
}





/**
  * @brief  Main program.
  * @param  None 
  * @retval None
  */
int main(void)
{  

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  // Initialize library
  ge_init();

  // Initialize LEDs
  setup_led_gpio();
  led_state = false;
  led_speed = false;

  // Initialize the USER button as an input
  gpio_setup_pin(GE_PBTN2, GPIO_INPUT, false, false);

  // Initialize PBTN1
  gpio_setup_pin(GE_PBTN1, GPIO_INPUT, false, false);

  // Print to serial port
  printf("Hello, World!\n");

  // Setup timer library
  // Set minimum timestep to 1ms (number of counts referecned to 
  // a 72MHz clock)
  timer_set_timestep(72000);
  // register callback for toggling LEDs every 500ms

  // led_timer = timer_register(500, &toggle_led, GE_PERIODIC);
  // timer_start(led_timer);

  watch_timer = timer_register(1, &update_time, GE_PERIODIC);
  timer_start(watch_timer);

  lcd_goto(0,0);
  lcd_puts("sup!");

  // set mode to the LED demo
  ui_state = LED_DEMO;

  // set pwm level for PWM demo
  float pwm_level = 0.0;

  // setup PWM library
  pwm_freq(10000.0);

  // setup ADC library
  // set sampling rate to 10kHz
  adc_set_fs(10000);
  // register callback method
  adc_callback(&my_adc_callback);
  // enable ADC channels
  adc_enable_channels(chan_to_conv, NUM_ADC);
  adc_initialize_channels();
  adc_start();

  // keep track of how many times the UI has looped
  num_refresh = 0;

  /* Infinite loop */
  /**
   * Handles the user interface state machine
   */
  while (1) {   

  if(!gpio_read_pin(GE_PBTN1))
  {
     stop = 0;
  }

  if(!gpio_read_pin(GE_PBTN2))
  {
     stop = 1;
  }

// reset 
  if(!gpio_read_pin(GE_PBTN3))
  {
    disp_time = 0;
  }

    int ms = disp_time%1000;
    int s = 0;

    if (disp_time > 1000)
      s  = (disp_time-ms)/1000;

    int min = s/60;


    s   = s - 60*min; 

    char buffer [20];
    snprintf(buffer, 20, "Time: %d:%d.%d",min,s,ms);
    
    lcd_clear();
    lcd_goto(0,0);
    lcd_puts(buffer);
    delay_ms(50);


  // while (!gpio_read_pin(GE_PBTN3));

  //   // check whether to change state
  //   if (!gpio_read_pin(GE_PBTN1)) {
  //     // stop LED timer if necessary
  //     if (ui_state == LED_DEMO) timer_stop(led_timer);

  //     ui_state++;
  //     if (ui_state >= NUM_STATES) ui_state = LED_DEMO;

  //     change_state();

  //     // wait for button to be released
  //     while (!gpio_read_pin(GE_PBTN1));
  //   }

  }
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
