#include <avr/io.h>
#include <avr/interrupt.h>
#include "../dac_driver.h"
#include "../dma_driver.h"
// The board.h header file defines which IO ports peripherals like
// Switches and LEDs are connected to. The header file is configured
// for use with XMEGA-A1 Xplained.
#include "../board.h"

#define SINE_WAVE_LOW_RES  20
#define SINE_WAVE_HIGH_RES 40

#define DMA_CHANNEL_0  &DMA.CH0

#define DMA_REPEAT_FOREVER 0
// 2MHz, 20/40 Samples, dividend of 200 gives 500Hz/250Hz
#define TIMER_C0_PERIOD 200

uint16_t SineWaveLowRes [SINE_WAVE_LOW_RES] =
                        {
                            0x7FF, 0xA78, 0xCB2, 0xE77, 0xF9A,
                            0xFFF, 0xF9A, 0xE77, 0xCB2, 0xA78,
                            0x7FF, 0x586, 0x34C, 0x187, 0x064,
                            0x000, 0x064, 0x187, 0x34C, 0x586
                        };

uint16_t SineWaveHighRes[SINE_WAVE_HIGH_RES] =
                        {
                            0x7FF, 0x93F, 0xA78, 0xBA1, 0xCB2,
                            0xDA7, 0xE77, 0xF1F, 0xF9A, 0xFE5,
                            0xFFF, 0xFE5, 0xF9A, 0xF1F, 0xE77,
                            0xDA7, 0xCB2, 0xBA1, 0xA78, 0x93F,
                            0x7FF, 0x6BF, 0x586, 0x45D, 0x34C,
                            0x257, 0x187, 0x0DF, 0x064, 0x019,
                            0x000, 0x019, 0x064, 0x0DF, 0x187,
                            0x257, 0x34C, 0x45D, 0x586, 0x6BF
                        };

ISR(TCC0_OVF_vect){}

void DMA_Setup( DMA_CH_t * dmaChannel,
                const void * src,
                void * dest,
                int16_t blockSize,
                uint8_t repeatCount )
{
    DMA_Enable();


	DMA_SetupBlock(
                    dmaChannel,
                    src,
                    DMA_CH_SRCRELOAD_BLOCK_gc,
                    DMA_CH_SRCDIR_INC_gc,
                    dest,
                    DMA_CH_DESTRELOAD_BURST_gc,
                    DMA_CH_DESTDIR_INC_gc,
                    blockSize,
                    DMA_CH_BURSTLEN_2BYTE_gc,
                    repeatCount,
                    true
                   );

    // Timer Overflow will trigger DMA
    DMA_SetTriggerSource( dmaChannel, 0x40 );
    DMA_EnableSingleShot( dmaChannel );
}


int main( void )
{
	// First we have to enable the audio amplifier by setting PQ3 high.
	PORTQ.PIN3CTRL = (PORTQ.PIN3CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;

	// Configure switches
	SWITCHPORTL.DIRCLR = 0x3F; // Set port as input

    PORTCFG.MPCMASK = 0x3F;
	SWITCHPORTL.PIN0CTRL = (SWITCHPORTL.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc; //Enable pull-up to get a defined level on the switches
    PORTCFG.MPCMASK = 0x3F;
    SWITCHPORTL.PIN0CTRL |= PORT_INVEN_bm;	// Inverted keys.. pressed = 1

    // Set direction as output for LEDs
    LEDPORT.DIR = 0xFF;

    // Invert all pins on LED_PORT
    PORTCFG.MPCMASK = 0xFF;
    LEDPORT.PIN0CTRL |= PORT_INVEN_bm;

    // Enable overflow interrupt
    TCC0.INTCTRLA = ( TCC0.INTCTRLA & TC0_OVFINTLVL_gm ) | TC_OVFINTLVL_MED_gc;

	DMA_Setup(DMA_CHANNEL_0, SineWaveHighRes, (void *) &DACB.CH0DATA, SINE_WAVE_HIGH_RES * 2, DMA_REPEAT_FOREVER);


	DAC_SingleChannel_Enable(
                                &DACB,
	                            DAC_REFSEL_AVCC_gc,
	                            false
	                        );

    DMA_EnableChannel( DMA_CHANNEL_0 );

	// Enable medium interrupt level in PMIC and enable global interrupts.
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;
    sei();

	while (1)
    {
        if(SWITCHPORTL.IN == 0x00)
        {
            // No Timer to trigger DMA: No Signal
             TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_OFF_gc;

        }
        else
        {
            // Enable Timer C0, prescaler div1 means Main Clock (2MHz).
            TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV1_gc;

    	    if(SWITCHPORTL.IN != 0x00)
            {
                TCC0.PER = TIMER_C0_PERIOD;

                while(SWITCHPORTL.IN != 0x00)
                {
                    LEDPORT.OUTSET = 0xFF;
                }
            }
            LEDPORT.OUT = 0x00;
        }
    }
}
