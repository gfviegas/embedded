/**
 * Gustavo Viegas (3026)
 *
 * Data: 16/09/19
 * Descrição: Piscar a led azul, acelerando/desacelerando alternadamente ao apertar o botão.
 * Hardware: Placa ESP32.
 */

#include <Arduino.h>
#include "BluetoothSerial.h"

#define SERVICE_UUID "713D0000-503E-4C75-BA94-3148F18D9422"
#define CHARACTERISTIC_WRITE_UUID "713D0001-503E-4C75-BA94-3148F18D9422"
#define CHARACTERISTIC_READ_UUID "713D0002-503E-4C75-BA94-3148F18D9422"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define LED 2
#define BUTTON 0

#define MODO_INCREMENT 0
#define MODO_DECREMENT 1

BluetoothSerial SerialBT;
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
 * Configura o dispositivo Bluetooth Serial
 */
void setupBluetooth() {
  Serial.begin(115200);
  SerialBT.begin("ViegasESP32");
  Serial.println("Dispositivo pronto para ser pareado!");
}

/**
 * Consulta se o serial recebeu um caracter 0 ou 1 para aumentar ou diminuir a velocidade
 */
void bluetoothHook() {
  char commandReceived;

  if (SerialBT.available()) {
    commandReceived = SerialBT.read();

    switch (commandReceived) {
      case '0':
        SerialBT.write('+');
        Serial.println("Diminuindo a velocidade");
        decrementDelay();
        break;
      case '1':
        SerialBT.write('-');
        Serial.println("Aumentando a velocidade");
        incrementDelay();
        break;
      default:
        break;
    }

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

  setupBluetooth();
}

/**
 * Hook de repetição
 */
void loop() {
  piscaLed();
  bluetoothHook();

  if (buttonState == LOW) updateDelay();
}
