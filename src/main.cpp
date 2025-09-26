#include <Arduino.h>

static QueueHandle_t adcQueue = nullptr;

// Read ADC on core 0 and push values to the queue
void readTask(void *pvParameters) {
  (void)pvParameters;
  const TickType_t delayTicks = pdMS_TO_TICKS(200);
  for (;;) {
    int sensorValue = analogRead(A0); // on ESP32 default ADC resolution is 12-bit (0-4095)
    xQueueSend(adcQueue, &sensorValue, 0);
    vTaskDelay(delayTicks);
  }
}

// Receive ADC values and print on core 1
void printTask(void *pvParameters) {
  (void)pvParameters;
  int sensorValue;
  for (;;) {
    if (xQueueReceive(adcQueue, &sensorValue, portMAX_DELAY) == pdTRUE) {
      // convert according to ADC resolution and reference (adjust if using attenuation)
      float voltage = sensorValue * (3.3f / 4095.0f);
      Serial.print("ADC Value: ");
      Serial.print(sensorValue);
      Serial.print("  Voltage: ");
      Serial.println(voltage, 3);
    }
    // small yield to avoid starving other tasks (not strictly necessary here)
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setup() {
  Serial.begin(115200);
  // create queue able to hold 10 int samples
  adcQueue = xQueueCreate(10, sizeof(int));
  if (!adcQueue) {
    Serial.println("Failed to create queue");
    while (1) { delay(1000); }
  }

  // Create tasks pinned to specific cores:
  // Reader on core 0, printer on core 1
  xTaskCreatePinnedToCore(readTask, "ReadTask", 2048, nullptr, 1, nullptr, 0);
  xTaskCreatePinnedToCore(printTask, "PrintTask", 4096, nullptr, 1, nullptr, 1);
}

void loop() {
  // empty - work happens in FreeRTOS tasks
  vTaskDelay(pdMS_TO_TICKS(1000));
}
