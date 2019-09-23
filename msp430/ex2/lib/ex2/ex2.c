#include "ex2.h"
#include <stdlib.h>

// Volátil para previnir otimização e manter o valo da variável entre as execuções
volatile unsigned int i = 0;
volatile unsigned int delayTime = 30000;
volatile int currentMode = 0;
volatile int activeLed = BIT0;
volatile int lastStatus = 1;
volatile int currentStatus = 1;

/**
 * Interrompe o watchdog timer que pode resetar o dispostivo depois de um certo período de tempo.
 */
void stopWatchdog(void) {
  WDTCTL = WDTPW + WDTHOLD;
}

/**
 * Define as direções dos pinos utilizados
 */
void setupPins(void) {
  // Define pins de output (leds). BIT0 = Led Vermelha, BIT6 = Led Verde.
  P1DIR |= (BIT0 + BIT6);

  // Habilita pullup/pulldown do pino 1.3 (0000 1000)
  P1REN = BIT3;
}

/**
 * Configura o inerrupt no push button
 */
void setupInterrupt(void) {
  // Habilita interrupt no P1.3
  P1IE = BIT3;
  P1IFG = 0x00;

  // Na borda de descida inicialmente
  P1IES = 0x00;

  // Habilita interrupts
  _BIS_SR(GIE);
}

/**
 * Aguarda até um loop atingir o tempo definido em delayTime
 */
void delay(void) {
  for (i = 0; i < delayTime; i++);
}

/**
 * Alterna o modo atual. Se estiver acelerando, seta para parar, e vice-versa.
 */
void toggleCurrentMode(void) {
  currentMode = !currentMode;
}

/**
 * ACELERA!!!!!
 */
void hitGas(int isPolling) {
  int limit = (isPolling) ? 1000 : 3500;
  delayTime = (delayTime <= 5000) ? limit : (delayTime - 5000);
}

/**
 * Pisa no freio zé!!!
 */
void hitBreak() {
  delayTime = (delayTime >= 20000) ? 20000 : (delayTime + 1000);
}

/**
 * Atualiza o valor do delayTime levando em conta o modo atual
 */
void updateDelayTime(int isPolling) {
  switch (currentMode) {
    case 1:
      hitGas(isPolling);
    case 0:
      hitBreak();
  }
}

/**
 * Alterna a led ligada.
 */
void switchLeds(void) {
  if (activeLed == BIT0) {
    P1OUT &= ~BIT6;     // Desliga GREEN
    P1OUT |= BIT0;			// Liga RED
    activeLed = BIT6;
  } else {
    P1OUT &= ~BIT0;			// Desliga RED
    P1OUT |= BIT6;			// Liga GREEN
    activeLed = BIT0;
  }
}

/**
 * Faz a led piscar usando uma operação XOR
 * L-1 | L | ^=
 *  0  | 1 | 1
 *  1  | 1 | 0
 */
void blinkLed(void) {
  switchLeds();
  delay();
  switchLeds();
  delay();
}

// Rotina de tratamento de interrupção da porta
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
  currentStatus = (P1IES != BIT3);
  if (currentStatus == 1) toggleCurrentMode();

  // Alterna a borda do interrupt
  P1IES = !currentStatus ? 0x00 : BIT3;
  P1IFG = 0x00;
}

/**
 * Executa a aplicação usando polling.
 */
void runPolling(void) {
  stopWatchdog();
  setupPins();

  while(1) {
    blinkLed();
    currentStatus = (P1IN & 0x08);
    // Se o botao tiver pressionado
    if (currentStatus == 0) {
      updateDelayTime(1);
    } else if (currentStatus != lastStatus) {
      toggleCurrentMode();
    }

    lastStatus = currentStatus;
  }
}

/**
 * Executa a aplicação usando interrupt.
 */
void runInterrupt(void) {
  stopWatchdog();
  setupPins();
  setupInterrupt();

  while(1) {
    blinkLed();

    // Se o botao tiver pressionado
    if (currentStatus == 0) updateDelayTime(0);
  }
}
