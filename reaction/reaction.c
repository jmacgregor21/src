#include "elecanisms.h"
#include "stdlib.h"

typedef void (*STATE_HANDLER_T)(void);

void start(void);
void choose(void);
void pull(void);
void twist(void);
void bop(void);
void shout(void);

STATE_HANDLER_T state, last_state;
uint16_t counter;
uint16_t twister;
uint16_t sound;
int count_num;

void start(void){
// state initialization
  if (state != last_state) {
    last_state = state;
    D10 = ON;
    D11 = ON;
    D12 = ON;
    counter = 0;
  }
//check for state transitions
  if (D0 == 0) {
      D1  = ON;
      state = choose;
  }
// leave state and clean up
  if (state != last_state) {
    D10 = OFF;
    D11 = OFF;
    D12 = OFF;
  }
}

void choose(void){
  if (state != last_state) {
    last_state = state;
  }
// perform state tasks
  if (counter >= 240) {
    state = start;
  }
  if (IFS0bits.T1IF == 1) {
    IFS0bits.T1IF = 0;
    counter++;
  }
  D13 = ON; // oscillator
  count_num = (int)(rand() % 4);
  if (count_num == 0) {
    state = pull;
  }
  if (count_num == 1) {
    state = twist;
  }
  if (count_num == 2) {
    state = bop;
  }
  if (count_num == 3) {
    state = shout;
  }
}

void pull(void) {
  if (state != last_state) {
    last_state = state;
    D9 = ON;
  }
// perform state tasks
  if (counter >= 240) {
    state = start;
  }
  if (IFS0bits.T1IF == 1) {
    IFS0bits.T1IF = 0;
    counter++;
  }
  D13 = ON; // oscillator
  // check for state transitions
  if (D8 == 1) {
    state = choose;
  }
  if (state != last_state) {
    D9 = OFF;
  }
}

void twist(void) {
  if (state != last_state) {
    last_state = state;
  }
// perform state tasks
  if (counter >= 240) {
    state = start;
  }
  if (IFS0bits.T1IF == 1) {
    IFS0bits.T1IF = 0;
    counter++;
  }
  if (counter > 512) {
    D7 = ON;
  }
  else if (counter < 512) {
    D6 = ON;
  }
  D13 = ON; // oscillator
  // check for state transitions
  if (D6 = ON && twister > 512) {
      state = choose;
  }
  if (D7 = ON && twister < 512){
      state = choose;
  }
  // leave state
  if (state != last_state) {
    D7 = OFF;
    D6 = OFF;
  }
}

void bop(void) {
  if (state != last_state) {
    last_state = state;
    D5 = ON;
  }
// perform state tasks
  if (counter >= 240) {
    state = start;
  }
  if (IFS0bits.T1IF == 1) {
      IFS0bits.T1IF = 0;
      counter++;
  }
  // check for state transitions
  if (D4 == 1) {
    D3 = OFF;
  }
  if (D4 == 0) {
    D3 = ON;
    state = choose;
  }

  if (state != last_state) {
    D5 = OFF; // led indicator
  }
}

void shout(void) {
  if (state != last_state) {
    last_state = state;
    D2 = ON;
  }
// perform state tasks
  if (counter >= 240) {
    state = start;
  }
  if (IFS0bits.T1IF == 1) {
      IFS0bits.T1IF = 0;
      counter++;
  }
  // check for state transitions
  if (sound > 720 || sound < 240) {
    state = choose;
  }
  // leave state
  if (state != last_state) {
    D2 = OFF;
  }
}

int16_t main(void) {
    init_elecanisms();
    twister = read_analog(5);
    sound = read_analog(3);
    T1CON = 0x0030;         // set Timer1 period to 0.5s
    PR1 = 0x7A11;

    TMR1 = 0;               // set Timer1 count to 0
    IFS0bits.T1IF = 0;      // lower Timer1 interrupt flag
    T1CONbits.TON = 1;      // turn on Timer1

    state = start;
    last_state = (STATE_HANDLER_T)NULL;

    while (1) {
        state();
    }
}

//speaker tone, didn't end up integrating since speaker isn't loud enough
// int16_t main(void) {
//     uint8_t *RPOR, *RPINR;
//
//     init_elecanisms();
//
//     // Configure pin D13 to produce a 1-kHz PWM signal with a 25% duty cycle
//     // using the OC1 module.
//     D13_DIR = OUT;      // configure D13 to be a digital output
//     D13 = 0;            // set D13 low
//
//     RPOR = (uint8_t *)&RPOR0;
//     RPINR = (uint8_t *)&RPINR0;
//
//     __builtin_write_OSCCONL(OSCCON & 0xBF);
//     RPOR[D13_RP] = OC1_RP;  // connect the OC1 module output to pin D13
//     __builtin_write_OSCCONL(OSCCON | 0x40);
//
//     OC1CON1 = 0x1C06;   // configure OC1 module to use the peripheral
//                         //   clock (i.e., FCY, OCTSEL<2:0> = 0b111) and
//                         //   and to operate in edge-aligned PWM mode
//                         //   (OCM<2:0> = 0b110)
//     OC1CON2 = 0x001F;   // configure OC1 module to syncrhonize to itself
//                         //   (i.e., OCTRIG = 0 and SYNCSEL<4:0> = 0b11111)
//
//     OC1RS = (uint16_t)(FCY / 1e3 - 1.);     // configure period register to
//                                             //   get a frequency of 1kHz
//     OC1R = OC1RS >> 1;  // configure duty cycle to 25% (i.e., period / 4) if doing >> 2
//     OC1TMR = 0;         // set OC1 timer count to 0
//
//     while(1) {
//
//       LED2 = ON;
//       //D13 = ON;
//
//     }
// }
