/*
 * usr_timer.c
 *
 *  Created on: Nov 26, 2017
 *      Author: chentuofei
 */

#include "usr_timer.h"
#include "usr_adc.h"
#include <stdio.h>

#define MEDIAN_FILTER_LEN 5

int median_filter1[MEDIAN_FILTER_LEN];
int median_filter2[MEDIAN_FILTER_LEN];
int median_filter3[MEDIAN_FILTER_LEN];
int median_filter4[MEDIAN_FILTER_LEN];

int median_cnt = 0;
int int_cnt = 0;
bool int_flag = 0;
bool blink = 0;


// A utility function to swap two elements
void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

/* This function takes last element as pivot, places
 the pivot element at its correct position in sorted
 array, and places all smaller (smaller than pivot)
 to left of pivot and all greater elements to right
 of pivot */
int partition(int arr[], int low, int high)
{
    int pivot = arr[high];    // pivot
    int i = (low - 1);  // Index of smaller element
    int j = low;
    for (j = low; j <= high - 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] <= pivot)
        {
            i++;    // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
 low  --> Starting index,
 high  --> Ending index */
void quickSort(int arr[], int low, int high)
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
         at right place */
        int pi = partition(arr, low, high);

        // Separately sort elements before
        // partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}
int getMedian(int arr [])
{
    quickSort(arr, 0, 4);//MEDIAN_FILTER_LEN-1);
    return arr[2];
}

void Timer0AIntHandler(void)
{
    // clear interrupt flag
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    float alpha = 0.3;
//    v_sense_main = alpha * v_sense_main + (1 - alpha) * adc_read(3, V_MAIN_PIN);
    v_sense_app1 = alpha * v_sense_app1 + (1 - alpha) * adc_read(3, V_APP1_PIN);
    v_sense_app2 = alpha * v_sense_app2 + (1 - alpha) * adc_read(3, V_APP2_PIN);

//    i_sense_main = alpha * i_sense_main + (1 - alpha) * adc_read(3, I_MAIN_PIN);
    i_sense_app1 = alpha * i_sense_app1 + (1 - alpha) * adc_read(3, I_APP1_PIN);
    i_sense_app2 = alpha * i_sense_app2 + (1 - alpha) * adc_read(3, I_APP2_PIN);

    float v_main = 0.0;
    float i_main = 0.0;
    float v_app1 = 0.0;
    float i_app1 = 0.0;
    float v_app2 = 0.0;
    float i_app2 = 0.0;

//   voltage calculation should move this somewhere else
//    v_sense_main = 3.3 * (float) (adc_read(3, V_MAIN_PIN) - v_main_offset)
//            / 4096; //*101.2/1.2;
//    v_sense_app1 = 3.3 * (float) (adc_read(3, V_APP1_PIN) - v_app1_offset)
//            / 4096; //*101.2/1.2;
//    v_sense_app2 = 3.3 * (float) (adc_read(3, V_APP2_PIN) - v_app2_offset)
//            / 4096; //*101.2/1.2;
//
////    current calculation
//    i_sense_main = (3.3 * (float) adc_read(3, I_MAIN_PIN) / 4096 - 2.5)
//            / 0.1024;
//    i_sense_app1 = (3.3 * (float) adc_read(3, I_APP1_PIN) / 4096 - 2.5)
//            / 0.2048;
//    i_sense_app2 = (3.3 * (float) adc_read(3, I_APP2_PIN) / 4096 - 2.5)
//            / 0.2048;

//    LEDWrite(CLP_D1, blink);
//    blink = !blink;

// capture the  max voltage and max current
    if (v_max_main < v_sense_main)
        v_max_main = v_sense_main;

    if (v_max_app1 < v_sense_app1)
        v_max_app1 = v_sense_app1;

    if (v_max_app2 < v_sense_app2)
        v_max_app2 = v_sense_app2;

    if (i_max_main < i_sense_main)
        i_max_main = i_sense_main;

    if (i_max_app1 < i_sense_app1)
        i_max_app1 = i_sense_app1;

    if (i_max_app2 < i_sense_app2)
        i_max_app2 = i_sense_app2;

    if (int_cnt > 100) // finished one cycle of the AC waveform
    {
        if (median_cnt < MEDIAN_FILTER_LEN)
        {
            median_filter1[median_cnt] = v_max_app1;
            median_filter2[median_cnt] = v_max_app2;
            median_filter3[median_cnt] = i_max_app1;
            median_filter4[median_cnt] = i_max_app2;
            median_cnt++;
        }

        else
        {
            median_cnt = 0;
            v_max_app1 = getMedian(median_filter1);
            v_max_app2 = getMedian(median_filter2);
            i_max_app1 = getMedian(median_filter3);
            i_max_app2 = getMedian(median_filter4);

            v_app1 =  1.174*3.3 * (float) (v_max_app1 - v_app1_offset) / 4096
                    * 56530 / 330;
            v_app2 =  1.174*3.3 * (float) (v_max_app2 - v_app2_offset) / 4096
                    * 56530 / 330;

            i_app1 = (3.3 * (float) i_max_app1 / 4096 - 2.5) / 0.2048;
            i_app2 = (3.3 * (float) i_max_app2 / 4096 - 2.5) / 0.2048;

            int_flag = 1;
            pwr_main = i_main * v_main;
            pwr_app1 = i_app1 * v_app1;
            pwr_app2 = i_app2 * v_app2;
        }

        //        reset max
        v_max_main = 0;
        v_max_app1 = 0;
        v_max_app2 = 0;
        i_max_main = 0;
        i_max_app1 = 0;
        i_max_app2 = 0;
        int_cnt = 0; // do something here by computing the Vrms and Irms
    }
    else
        int_cnt++;

}

void timer_init(uint32_t freq)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    //
    // Wait for the Timer0 module to be ready.
    //
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }

    // just a periodic timer
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    TimerLoadSet(TIMER0_BASE, TIMER_A, freq);

    UARTprintf("Timer: Interrupt at %d for ADC capture\n", freq);

    TimerIntRegister(TIMER0_BASE, TIMER_A, &Timer0AIntHandler);

    //
    // Enable processor interrupts.
    //

    IntMasterEnable();

    // Configure the Timer0B interrupt for timer timeout.
    //
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Enable the Timer0B interrupt on the processor (NVIC).
    //
    IntEnable(INT_TIMER0A);

    //
    // Enable Timer0B.
    //
    TimerEnable(TIMER0_BASE, TIMER_A);

}
