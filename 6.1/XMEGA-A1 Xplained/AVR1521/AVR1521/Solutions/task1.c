/*************************************************************************
 *
 *
 *              Task1: Introductory sleep-example
 *
 *
 *************************************************************************/

// Include header files for GCC/IAR
#include "../avr_compiler.h"

// The board.h header file defines which IO ports peripherals like
// Switches and LEDs are connected to. The header file is configured
// for use with XMEGA-A1 Xplained.
#include "../board.h"

enum SWITCHES { SW_ACTIVE = PIN0_bm, SW_IDLE = PIN1_bm, SW_STBY = PIN2_bm, SW_PWDN = PIN3_bm };

// The SLEEP-instruction
#define sleep() __asm__ __volatile__ ("sleep")

void facilitatePowersaving(void);

int main(void)
{
	facilitatePowersaving();
	
	// Configure switches
	PORTCFG.MPCMASK = 0xff; // Configure several PINxCTRL registers at the same time
	SWITCHPORTL.PIN0CTRL = (SWITCHPORTL.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;

	SWITCHPORTL.DIRCLR = 0xff; // Set port as input

	// Set up interrupt on buttons
	SWITCHPORTL.INTCTRL = (SWITCHPORTL.INTCTRL & ~PORT_INT0LVL_gm) | PORT_INT0LVL_LO_gc;
	SWITCHPORTL.INT0MASK = 0x0F;

	// Configure LEDs
	PORTCFG.MPCMASK = 0xff; // Configure several PINxCTRL registers at the same time
	LEDPORT.PIN0CTRL = PORT_INVEN_bm; // Invert input to turn the leds on when port output value is 1

	LEDPORT.DIRSET = 0xff; 	// Set port as output
	LEDPORT.OUT = 0x02;  // Set initial value


	// Enable low interrupt level in PMIC and enable global interrupts.
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();

	// Main loop.
	char pressed = 0;
	while (1) {
// == Button 0 pressed
		if (((SWITCHPORTL.IN & SW_ACTIVE) | pressed) == 0x00) {		
			_delay_ms(5); // Debounce switch
			pressed = 1; // Used for toggling
			LEDPORT.OUTSET = SW_ACTIVE; // Toggle led

			SLEEP.CTRL &= ~SLEEP_SEN_bm;	// Disable sleep


// == Button 1 pressed
		} else if (((SWITCHPORTL.IN & SW_IDLE) | pressed) == 0x00) {	
			_delay_ms(5); // Debounce switch
			pressed = 1; // Used for toggling
			LEDPORT.OUT = SW_IDLE; // Toggle led

			SLEEP.CTRL = (SLEEP.CTRL & ~SLEEP_SMODE_gm) | SLEEP_SMODE_IDLE_gc;
			SLEEP.CTRL |= SLEEP_SEN_bm; // Enable sleep, else SLEEP-instruction is ineffective.

 // == Button 2 pressed
		} else if (((SWITCHPORTL.IN & SW_STBY) | pressed) == 0x00) {
			_delay_ms(5); // Debounce switch
			pressed = 1; // Used for toggling.
			LEDPORT.OUT = SW_STBY; // Toggle led		


			SLEEP.CTRL = (SLEEP.CTRL & ~SLEEP_SMODE_gm) | SLEEP_SMODE_STDBY_gc;
			SLEEP.CTRL |= SLEEP_SEN_bm; // Enable sleep

// == Button 3 pressed
		} else if (((SWITCHPORTL.IN & SW_PWDN) | pressed) == 0x00) {
			_delay_ms(5); // Debounce switch
			pressed = 1; // Used for toggling.
			LEDPORT.OUT = SW_PWDN;


			SLEEP.CTRL = (SLEEP.CTRL & ~SLEEP_SMODE_gm) | SLEEP_SMODE_PDOWN_gc;
			SLEEP.CTRL |= SLEEP_SEN_bm; // Enable sleep

// == Buttons released
		} else if ( (SWITCHPORTL.IN & (SW_ACTIVE | SW_IDLE | SW_STBY | SW_PWDN)) == 0x0F) {
			pressed = 0;
			LEDPORT.OUT = 0x00;
			sleep();
		}


	}
	
}

ISR(SWITCHPORT_INT0_vect) {
	// Empty
}

void facilitatePowersaving(void) {
	// Pull-up on all ports, to ensure a defined state.
	PORTCFG.MPCMASK = 0xFF;
	PORTA.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTB.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTC.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTD.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTE.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTF.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTH.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTJ.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTK.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTQ.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTR.PIN0CTRL = PORT_OPC_PULLUP_gc;

	// Because of the audio amplifier, we have to define pull-down on PQ3
	// NTC is already disabled via pull-up
	PORTQ.PIN3CTRL = (PORTQ.PIN3CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLDOWN_gc;

	// The potentiometer has an analog voltage, thus both pull-up and pull-down in tristate will source current. ~35�A.
	// Do something clever to avoid current flowing out. See the solution for solution.
	// .. Add code for this here. See the schematics to find out which pin.
	PORTB.DIRSET = PIN1_bm;
	

}

