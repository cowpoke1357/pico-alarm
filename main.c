#include <stdio.h>
#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include "pico/sleep.h"


int i;
datetime_t *t_ptr;
static bool awake;

static void rtc_sleep();
static void sleep_callback();
static void rtc_alarm_repeat();

int main(){
    stdio_init_all();
    
//wait a second for connect via terminal
    sleep_ms(3000);
    printf("hello terminal\n");

    printf("switching to crystal oscillator\n");

    uart_default_tx_wait_blocking();
    sleep_run_from_xosc();
    awake = false;

    rtc_sleep();
  
    return 0;
}

static void rtc_sleep(){
    //set current time
    datetime_t t = {
        .year = 2024,
        .month = 7, //july
        .day = 05, //5
        .dotw = 05, //friday
        .hour = 14, //2pm
        .min = 50,
        .sec = 00,
    };

    //set alarm time
    datetime_t t_alarm = {
        .year = 2024,
        .month = 7, //july
        .day = 05, //5
        .dotw = 05, //friday
        .hour = 14, //2pm
        .min = 50,
        .sec = 10, //10 seconds from "current time"
    };

    t_ptr = &t_alarm;

    //initialize RTC
    rtc_init();
    rtc_set_datetime(&t);
    sleep_us(64);

    printf("sleeping for 10 seconds\n");
    uart_default_tx_wait_blocking();
    sleep_goto_sleep_until(&t_alarm, &sleep_callback);
}

static void sleep_callback(){
    printf("RTC woke me up :)\n");
    awake = true;
    i++;
    rtc_alarm_repeat();
}

static void rtc_alarm_repeat(){
    if(i < 3){
    t_ptr->sec += 10;
    sleep_goto_sleep_until(t_ptr, &sleep_callback);
    }
}