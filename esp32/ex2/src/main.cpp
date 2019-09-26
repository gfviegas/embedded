/**
 * Gustavo Viegas (3026)
 *
 * Data: 16/09/19
 * Descrição: Piscar a led azul, acelerando/desacelerando alternadamente ao apertar o botão.
 * Hardware: Placa ESP32.
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "BluetoothSerial.h"

#define SERVICE_UUID "713D0000-503E-4C75-BA94-3148F18D9422"
#define CHARACTERISTIC_UUID  "713D0001-503E-4C75-BA94-3148F18D9422"
#define CHARACTERISTIC2_UUID "713D0002-503E-4C75-BA94-3148F18D9422"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define LED 2
#define BUTTON 0

#define MODO_INCREMENT 0
#define MODO_DECREMENT 1

BLECharacteristic *pChar;
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

class BluetoothCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      // Serial.println(value)
      if (value[0] == '+') {
        incrementDelay();
      } else {
        decrementDelay();
      }


      pChar->setValue((uint8_t*)&delayTime, 4);
      pChar->notify();
    }
};

/**
 * Configura o dispositivo Bluetooth Serial
 */
void setupBluetooth() {
  Serial.begin(115200);

  BLEDevice::init("ViegasESP32_BLE");
  
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE|
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );
  pChar = pService->createCharacteristic(
                                         CHARACTERISTIC2_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE|
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );
  pChar->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new BluetoothCallbacks());

  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  Serial.println("Dispositivo pronto para ser pareado!");
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
  // bluetoothHook();

  if (buttonState == LOW) updateDelay();
}
