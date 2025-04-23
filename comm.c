#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"

void auto_brake(int devid)
{
    // Task-1: 
    // Your code here (Use Lab 02 - Lab 04 for reference)
    // Use the directions given in the project document

    if ('Y' == ser_read(devid) && 'Y' == ser_read(devid)) 
    {
        uint8_t low_byte = ser_read(devid); //  low byte
        uint8_t high_byte = ser_read(devid); // high byte

        //for(int i = 0; i < 5; i++) {ser_read(devid);};

        uint16_t dist = low_byte | (high_byte << 8); // compute bytes to get dist 
        printf("Distance: %d cm \n", dist);

        // if conditions 
        if (dist > 200) {
            gpio_write(GREEN_LED, ON);
            gpio_write(RED_LED, OFF);
            gpio_write(BLUE_LED, OFF);
        }

        else if( dist <= 200 && dist > 100)
        {
            gpio_write(GREEN_LED, ON);
            gpio_write(RED_LED, ON);
            gpio_write(BLUE_LED, OFF); 
        }

        else if(dist > 60 && dist <= 100)
        {
            gpio_write(GREEN_LED, OFF);
            gpio_write(RED_LED, ON);
            gpio_write(BLUE_LED, OFF); 
        }

        else if(dist <= 60)
        {
            gpio_write(GREEN_LED, OFF);
            gpio_write(BLUE_LED, OFF);
            gpio_write(RED_LED, OFF); //turn turn off red led
            delay(100); // delay time
            gpio_write(RED_LED, ON); // to turn on red led

        }
    }
}

int read_from_pi(int devid)
{
    if(ser_isready(devid)){
        int angle;
        char data[100];
        ser_readline(devid, 100, data);
        sscanf(data, "%d", &angle);
        return angle;
    }
    return 0;
}

void steering(int gpio, int pos)
{
    int pwm_signal = 544 + ((2400-544) * pos)/180;
    gpio_write(gpio, ON);
    delay_usec(pwm_signal);
    gpio_write(gpio, OFF);
    delay_usec(20000-pwm_signal);
}


int main()
{
    // initialize UART channels
    ser_setup(0); // uart0
    ser_setup(1); // uart1
    int pi_to_hifive = 1; //The connection with Pi uses uart 1
    int lidar_to_hifive = 0; //the lidar uses uart 0

    int gpio = PIN_19;
    printf("\nUsing UART %d for Pi -> HiFive", pi_to_hifive);
    printf("\nUsing UART %d for Lidar -> HiFive", lidar_to_hifive);
    
    //Initializing PINs
    gpio_mode(PIN_19, OUTPUT);
    gpio_mode(RED_LED, OUTPUT);
    gpio_mode(BLUE_LED, OUTPUT);
    gpio_mode(GREEN_LED, OUTPUT);


    printf("Setup completed.\n");
    printf("Begin the main loop.\n");

    while (1) {

        auto_brake(lidar_to_hifive); // measuring distance using lidar and braking
        int angle = read_from_pi(pi_to_hifive); //getting turn direction from pi
        printf("\nangle=%d", angle) 
        for(int i = 0; i < 10; i++){
            if(angle>0){
                steering(gpio, 180);
            }
            else{
                steering(gpio, 0);
            }
    }

    }
    return 0;
}
