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

        for(int i = 0; i < 5; i++) ser_read(devid);
        
        uint16_t dist = (high_byte << 8) | low_byte; // compute bytes to get dist 
        printf("Distance: %d cm \n", dist);
    
        // set LED mode to OUTPUT
        gpio_mode(RED_LED,OUTPUT);
        gpio_mode(GREEN_LED,OUTPUT);
        gpio_mode(BLUE_LED,OUTPUT);


        // if conditions 
        if (dist > 200) {
            gpio_write(GREEN_LED, ON);
            gpio_write(RED_LED, OFF);
            gpio_write(BLUE_LED, OFF);
        }

        if( dist <= 200 && dist > 100)
        {
            gpio_write(GREEN_LED, ON);
            gpio_write(RED_LED, ON);
            gpio_write(BLUE_LED, OFF); 
        }

        if(dist > 60 && dist <= 100)
        {
            gpio_write(GREEN_LED, OFF);
            gpio_write(RED_LED, ON);
            gpio_write(BLUE_LED, OFF); 
        }

        else
        {
            int red_status = OFF;
            uint64_t last_toggle = 0;
            uint64_t current_cycles = get_cycles(); // retrieves the current cycle
            uint64_t cycles_100ms = 32768/10; // calculates the hz to convert to ms

            if(current_cycles - last_toggle >= cycles_100ms)
            {
                red_status = ON;
                gpio_write(GREEN_LED, OFF);
                gpio_write(RED_LED,red_status);
                gpio_write(BLUE_LED, OFF); 
                last_toggle = current_cycles;
            }

        }
    }
}

int read_from_pi(int devid)
{
    // Task-2: 
    // You code goes here (Use Lab 09 for reference)
    // After performing Task-2 at dnn.py code, modify this part to read angle values from Raspberry Pi.

}

void steering(int gpio, int pos)
{
    // Task-3: 
    // Your code goes here (Use Lab 05 for reference)
    // Check the project document to understand the task
}


int main()
{
    // initialize UART channels
    ser_setup(0); // uart0
    ser_setup(1); // uart1
    int pi_to_hifive = 1; //The connection with Pi uses uart 1
    int lidar_to_hifive = 0; //the lidar uses uart 0
    
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
        int gpio = PIN_19; 
        for (int i = 0; i < 10; i++){
            // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
            // and 0 if the prediction is a negative angle.
            // This is so that it is easier to see the movement of the servo.
            // You are welcome to pass the angle values directly to the steering function.
            // If the servo function is written correctly, it should still work,
            // only the movements of the servo will be more subtle
            if(angle>0){
                steering(gpio, 180);
            }
            else {
                steering(gpio,0);
            }
            
            // Uncomment the line below to see the actual angles on the servo.
            // Remember to comment out the if-else statement above!
            // steering(gpio, angle);
        }

    }
    return 0;
}
