/*
 * PosturePatch Phase 1 - Raw HX711 Data Streaming
 * 
 * Hardware Configuration:
 * - ESP32 MCU (ESP-IDF target: esp32 or esp32s3)
 * - HX711 24-bit ADC with foil strain gauges in Wheatstone bridge
 * - Wiring:
 *   * HX711_VCC -> 3V3
 *   * HX711_GND -> GND  
 *   * HX711_DOUT -> ESP32 GPIO 19
 *   * HX711_SCK  -> ESP32 GPIO 18
 * 
 * Usage:
 * 1. cd firmware
 * 2. idf.py set-target esp32
 * 3. idf.py build
 * 4. idf.py -p <YOUR_SERIAL_PORT> flash monitor
 * 
 * Expected Output:
 * - CSV format: timestamp_us,raw
 * - ~80 Hz sampling rate
 * - Raw values fluctuate around mean at rest
 * - Values change predictably when force applied
 */

#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "hx711.h"

// === CONFIG: update to actual pins ===
#define PIN_DOUT 19
#define PIN_SCK  18
#define LOOP_DELAY_MS 10   // ~100Hz loop; HX711 ready gating will set actual print cadence

void app_main(void) {
    hx711_t hx;
    hx711_init(&hx, PIN_DOUT, PIN_SCK);

    // Print CSV header
    printf("timestamp_us,raw\n");

    while (1) {
        int32_t raw;
        if (hx711_read(&hx, &raw)) {
            int64_t ts = esp_timer_get_time();
            printf("%" PRId64 ",%ld\n", ts, (long)raw);
        }
        vTaskDelay(pdMS_TO_TICKS(LOOP_DELAY_MS));
    }
}
