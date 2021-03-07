#include <msp430g2553.h>
#include "lcd.h"
int a;
int n;
int g;
int sure;
long sensor;
int distance;
int miliseconds;
#define SERVO_BIT BIT6
#define SM_clk 1100000
#define servo_freq 50
int PWM_period = SM_clk /servo_freq;
void servo_init (){
 P1DIR |= SERVO_BIT
 P1SEL |= SERVO_BIT;
 TACCR0 = PWM_period-1;
 TACCR1 = 0;
 TACCTL1 = OUTMOD_7;
 TACTL=TASSEL_2+MC_1;
}
void main(void)
{
 BCSCTL1 = CALBC1_1MHZ;
 DCOCTL = CALDCO_1MHZ;
 WDTCTL = WDTPW + WDTHOLD;
 CCTL0 = CCIE;
 CCR0 = 1000;
 TACTL = TASSEL_2 + MC_1;
 P2IFG = 0x00;
 P1DIR |= 0x01;
 P1DIR = BIT0;
 P1OUT &= ~0x01;
 P2DIR = BIT2+BIT1+BIT3;
 __enable_interrupt();
while(1){
 P1IE &= ~0x01;
 P2DIR |= BIT5;
 P2OUT |= BIT5;
 __delay_cycles(10);
 P2OUT &= ~BIT5;
 P2DIR &= ~BIT4;
 P2IFG = 0x00;
 P2IE |= BIT4;
 P2IES &= ~BIT4;
 __delay_cycles(30000);
 distance = sensor/58;
 if(n > 50 && TACCR1 >= 1900){
 for (g=10; g>= 0; g--)
 {
 lcd_init();
 send_string("door closing...");
 send_command(0xC0);
 send_int(g);
 __delay_cycles(350000);
 if(g==0)
 {
 lcd_init();
 send_string("");
 send_command(0xC0);
 }
 }
 }
 if(a < 1000)
 {
 if(distance < 4)
 {
 while(1) {
 P2OUT = BIT2+BIT1;
 __delay_cycles(30000);
 P2OUT = ~BIT2;
 P2OUT = BIT1;
 servo_init();
 TACCR1 = 1900;
 a=0;
 break;
 }
 }
 }
 else
 {
 while(1){
 P2OUT = BIT2+BIT3;
 __delay_cycles(30000);
 P2OUT = ~BIT2;
 P2OUT = BIT3;
 servo_init();
 TACCR1 = 1050;
 a = 0;
 break;
 }
 }
 }
 }
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
 if(P2IFG&BIT4)
 {
 if(!(P2IES&BIT4))
 {
 TACTL|=TACLR;
 miliseconds = 0;
 P2IES |= BIT4; 
 }
 else
 {
 sensor = (long)miliseconds*1000 + (long)TAR;
 }
 P2IFG &= ~BIT4;
 }
}
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
 miliseconds++;
 a++;
 n = a/17;
}
