#include <Arduino.h>

uint8_t i = 0;

void setup()  
{
  

  digitalWrite(RED_LED, LOW);    // set the LED off
  pinMode(RED_LED, OUTPUT);     
  pinMode(GREEN_LED, OUTPUT);     
  
  Serial.begin();
  Serial.println("Hello USB world");
}

void loop() // run over and over
{
  if (Serial.available())
  {
    Serial.write(Serial.read());
    i ^= HIGH;                   // Toggle value
    digitalWrite(RED_LED, i);    // toggle the LED
  }

} 