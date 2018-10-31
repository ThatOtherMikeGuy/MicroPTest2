/* S2017Ex02Pr02.c
referenced ctServoPWMPlusPlus.c from the student drive
Mike Schwindt
 */
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
uint32_t  pulseWidth;   
uint32_t  deltaWidth;   // Currently static, amount of change per P1.3
uint8_t	  step;
bool up;

void __attribute__((interrupt(PORT1_VECTOR))) Sw2ISR(void)
{
	step = (P1IN & BIT3);
    switch(step) {
        case 0: // switch 2 is pressed
			if(up)
			{
				pulseWidth += deltaWidth;
				if(pulseWidth>=32000) 
				{
					pulseWidth=32000;
					up = false;
				}
			}
			else
			{
				pulseWidth -= deltaWidth;
				if(pulseWidth<=8000) 
				{
					pulseWidth=8000;
					up = true;
				}
			}
            break;
        default: 
            ;// No change
    }
    TA0CCR1=pulseWidth;
    __delay_cycles(200000L); // Debounce delay, ignore switch after H->L
    P1IFG &= ~BIT3;          // This is a 0.2 s delay before clearing int flag.
    return;
}

void initSW2int(void) {
    P1DIR &= ~BIT3; // We use SW2 as a user input to move the
    P1OUT |= BIT3;  // servo stepwise.
    P1REN |= BIT3;  // Pull-up resistor enabled.
    P1IES |= BIT3;  // High to low edge sensitivity.
    P1IE  |= BIT3;  // Respond with interrupt.
}
void initPWM(void) {
    P1DIR |= BIT6;  // Direct PWM output on P1.6 (pin 14)
    P1OUT |= BIT6;
    P1SEL |= BIT6;   // Need to enable TA0.1 output at P1.6,
    P1SEL2 &= ~BIT6; // see datasheet schematic for P1.6 and following table.
    //
    pulseWidth = 8000;  // Initial pulsewidth is 8 ms, steps of 6 ms.
    deltaWidth = 6000;
    TA0CCTL1 = OUTMOD_6;
    TA0CCR1  =  pulseWidth;
    TA0CCR0  = 40000;   // The static pulse period is 40 ms.
    TA0CTL   = (TASSEL_2 | ID_0 | MC_1); // SMCLK (1MHz), no division, Up Mode.
	up = true;
    return;
}

int main(void) 
{
    WDTCTL  = WDTPW | WDTHOLD;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL  = CALDCO_1MHZ;
    initSW2int();
    initPWM();
    __eint();
    while(1) 
	{
         __bis_status_register(LPM1_bits); // See msp430g2553.h
    }
    return 0;
}
