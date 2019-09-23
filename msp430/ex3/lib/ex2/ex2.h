#include <msp430g2553.h>

void stopWatchdog(void);
void setupPins(void);
void setupInterrupt(void);
void delay(void);
void toggleCurrentMode(void);
void hitGas(int isPolling);
void hitBreak();
void updateDelayTime(int isPolling);
void switchLeds(void);
void blinkLed(void);
void runPolling(void);
void runInterrupt(void);
// __interrupt void Port_1(void);
