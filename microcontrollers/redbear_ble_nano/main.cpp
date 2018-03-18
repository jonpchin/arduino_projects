#include "mbed.h"

//GND, VIN, SWCLK & SWDIO are the only pins needed for programming on BLE Nano
int main(void){
    mbed::PwmOut pwm_b(P0_8);
    mbed::DigitalOut gpio_b(P0_11);
    mbed::PwmOut pwm_a(P0_9);
    mbed::DigitalOut gpio_a(P0_10);
    mbed::DigitalOut mode_pin(P0_5);
    mode_pin.write(1);
    pwm_b.write(0.2);
    gpio_b.write(0);
    pwm_a.write(1.0);
    gpio_a.write(0);
    wait(5);
    gpio_b.write(1);
    gpio_a.write(1);
    wait(5);
    pwm_b.write(0);
    pwm_a.write(0);
    return 0;    
}
    