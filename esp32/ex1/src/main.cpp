/**
 * Gustavo Viegas (3026)
 *
 * Data: 16/09/19
 * Descrição: Piscar a led azul, acelerando/desacelerando alternadamente ao apertar o botão.
 * Hardware: Placa ESP32.
 */

#include <Arduino.h>
#define LED 2
#define BUTTON 0

/**
 * Exemplo usando interrupt.
 */
#define MODO_INCREMENT 0
#define MODO_DECREMENT 1

int delayTime = 800;
int buttonState = HIGH;
int modoAtual = MODO_DECREMENT;

/**
 * Alterna o modo atual de aceleração do delay
 */
void toggleModoAtual() {
  modoAtual = (modoAtual == MODO_INCREMENT) ? MODO_DECREMENT : MODO_INCREMENT;
}

/**
 * Aumenta a quantidade do delay até um limite (2000ms)
 */
void incrementDelay() {
  if (delayTime >= 2000) return;
  delayTime += 150;
}

/**
 * Diminui a quantidade do delay até um limite (100ms)
 */
void decrementDelay() {
  if (delayTime <= 50) return;
  delayTime -= 150;
}

/**
 * Pisca-se a led azul
 */
void piscaLed() {
  delay(delayTime);
  digitalWrite(LED, HIGH);
  delay(delayTime);
  digitalWrite(LED, LOW);
}

/**
 * Atualiza a quantidade de delay de acordo com o modo atual
 */
void updateDelay() {
  if (modoAtual == MODO_INCREMENT) {
    incrementDelay();
  } else {
    decrementDelay();
  }
}

/**
 * Avalia a mudança do botão
 */
void handleButtonChange () {
  buttonState = digitalRead(BUTTON);

  if (buttonState == HIGH) {
    toggleModoAtual();
  }
}

/**
 * Hook de configuração
 */
void setup() {
  //  Inicializa as leds como output
  pinMode(LED, OUTPUT);

  //  Inicializa o botão como input
  pinMode(BUTTON, INPUT_PULLUP);

  // Adiciona interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON), handleButtonChange, CHANGE);
}

/**
 * Hook de repetição
 */
void loop() {
  piscaLed();

  if (buttonState == LOW) updateDelay();
}
