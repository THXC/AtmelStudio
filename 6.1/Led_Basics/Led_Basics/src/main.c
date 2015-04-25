/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#define LEDPORT PORTE

volatile unsigned int counter;

int main( void )
{
   LEDPORT.DIR = 0xff;    // Set all pins of port D to output.
   LEDPORT.OUT = 0xf0;    // Set upper four bits high.
   LEDPORT.OUTSET = 0x0f; // Set lower four bits high, do not touch others.
   LEDPORT.OUTCLR = 0xf0; // Clr upper four bits, do not touch others.
   LEDPORT.OUTTGL = 0xff; // Toggle all pins, ones become zeros and vice versa.
   LEDPORT.OUTTGL = 0xff; // Toggle all pins, ones become zeros and vice versa.
   LEDPORT.OUTTGL = 0xAA; // Toggle some pins, ones become zeros and vice versa.
   LEDPORT.OUTTGL = 0xAA; // Toggle some pins, ones become zeros and vice versa.
   LEDPORT.OUTTGL = 0xAA; // Toggle some pins, ones become zeros and vice versa.
   LEDPORT.OUTTGL = 0xAA; // Toggle some pins, ones become zeros and vice versa.
	
   // Now, just for fun, we toggle all LEDs, again and again and again...
   while (1)
   {
        LEDPORT.OUTTGL = 0xff;
        // Delay
        for ( counter=0; counter<10000; counter++) 
		{
			nop();
		}
   }
}