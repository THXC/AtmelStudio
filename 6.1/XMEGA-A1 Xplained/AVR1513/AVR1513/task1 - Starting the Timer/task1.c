// Setup for different compilers
#include "../avr_compiler.h"
#include "stdint.h"

// The board.h header file defines which IO ports peripherals like
// Switches and LEDs are connected to. The header file is configured
// for use with XMEGA-A1 Xplained.
#include "../board.h"

/*This code use Timer/Counter C0: TCC0 */

int main( void )
{
  
  /* To toggle two LEDs, set pin 0 and 1 as output. 
   * In addition one pin is default high and one is default low
   * so they toggle out of phase */
  
  LEDPORT.DIRSET = PIN0_bm | PIN1_bm;
  LEDPORT.OUTSET = PIN1_bm; 
  
  /* The Period (PER) register sets the TOP value for the counter, 
   * i.e. how far it will count */    

  TCC0.PER = 0x30;
  
  /* The CLKSEL bits in CTRLA is used to set the clock source for the counter */
  
  TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV1_gc;  // Start Timer with no prescaling
 
    
  while (1)
  {
    /* Check if the overflow flag (OVFIF) is set,
     * clear flag and toggle LEDs
     */
    
    if((TCC0.INTFLAGS & TC0_OVFIF_bm) != 0)
    {
      TCC0.INTFLAGS = TC0_OVFIF_bm;			//Clear the IF by writing a logical 1 to the flag
      LEDPORT.OUTTGL = PIN0_bm | PIN1_bm;	//Toggle pin 0 and 1 on the LED port.
    }   
	
  }
  
}
