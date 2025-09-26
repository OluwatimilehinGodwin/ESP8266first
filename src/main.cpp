#include <Arduino.h>

int sensorValue = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  sensorValue = analogRead(A0); // 0 - 1023 (10-bit ADC)
  float voltage = sensorValue * (3.3 / 1023.0); // if board supports 0–3.3V
  Serial.print("ADC Value: "); Serial.print(sensorValue);
  Serial.print("  Voltage: "); Serial.println(voltage);
  delay(500);
}

