/* S2017Ex02Pr01.c
referenced ctServoPWMPlusPlus.c from the student drive
Mike Schwindt
 */
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
uint16_t  pulseWidth;   // Must be variable, changes with P1.3 inputs.
uint16_t  deltaWidth;   // Currently static, amount of change per P1.3.
uint8_t   doWhat;       // Determined from inputs at P1.1 and P1.0 .
uint8_t   redCount = 0;


void __attribute__((interrupt(PORT1_VECTOR))) Sw2ISR(void) {
    doWhat = (P1IN & (BIT3));
    switch(doWhat) {
        case 0: //sw2 pushed 
			redCount = redCount + 1;
			P1OUT ^= BIT6; // toggle gren led
			if(redCount == 4)
			{
				P1OUT ^= BIT0; // toggle red led
				redCount = 0;
			}
            break;
        default: 
			;// No change
    }
    __delay_cycles(200000L); // Debounce delay, ignore switch after H->L
    P1IFG &= ~BIT3;          // This is a 0.2 s delay before clearing int flag.
	//P1IE  |=  BIT3; 
    return;
}

void initSW2int(void) {
    P1DIR &= ~BIT3; // We use SW2 as a user input to count/toggle
    P1OUT |= BIT3;  
    P1REN |= BIT3;  // Pull-up resistor enabled.
    P1IES &= ~BIT3;  // Low to high edge sensitivity.
    P1IE  |= BIT3;  // Respond with interrupt.
}
void initGREENled(void) {
    P1DIR |= BIT6;  // P1.6 (Green led) is an output
    P1OUT &= ~BIT6;  // green is low to start

    return;
}

void initREDled() {
    P1DIR |= BIT0;    // P1.0 (RED LED) is an output.
    P1OUT |= BIT0;    // red is high to start

	return;
}

int main(void) {
    WDTCTL  = WDTPW | WDTHOLD;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL  = CALDCO_1MHZ;
    initSW2int();
	initREDled();
    initGREENled();
    __eint();
    while(1) {
         __bis_status_register(LPM4_bits); // See msp430g2553.h
    }
    return 0;
}