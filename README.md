# PosturePatchFirmware

Firmware for PosturePatch, a wearable posture sensor. Phase 1 reads foil strain
gauges through an HX711 24-bit ADC on an ESP32-C3 and streams the raw readings
over serial as CSV, as the first step towards measuring how the back bends.

The earlier prototype, an IMU with an on-device TinyML classifier, is in
[PostureCheckerAI](https://github.com/VyomSawhney/PostureCheckerAI).

## Hardware

- ESP32-C3, built with ESP-IDF 5.2
- HX711 24-bit ADC with foil strain gauges wired as a Wheatstone bridge

| HX711 | ESP32-C3 |
| ----- | -------- |
| VCC   | 3V3      |
| GND   | GND      |
| DOUT  | GPIO 19  |
| SCK   | GPIO 18  |

The pins are set at the top of `firmware/main/main.c`.

## What it does

`main.c` polls the HX711 every 10 ms. Whenever a conversion is ready, it prints
one line:

```
timestamp_us,raw
1234567,-81234
...
```

`timestamp_us` comes from the ESP high-resolution timer. `raw` is the signed
24-bit reading. At rest the values sit around a constant offset, and they move
predictably when force is applied to the gauges. The output rate follows the
HX711's own data rate, 10 or 80 samples per second depending on its RATE pin.

## The HX711 driver

`firmware/main/hx711.c` is a small bit-banged driver with no dependencies
beyond ESP-IDF's GPIO and ROM delay functions.

- **Ready check.** The HX711 pulls DOUT low when a conversion is ready.
- **Read.** 24 clock pulses on SCK shift the result out MSB first, with a 1 µs
  delay on each edge.
- **Gain.** A 25th pulse selects channel A at gain 128 for the next conversion.
- **Sign.** The 24-bit two's-complement result is sign-extended to 32 bits.

## Building

With ESP-IDF 5.2 installed and exported:

```
cd firmware
idf.py set-target esp32c3
idf.py build
idf.py -p <PORT> flash monitor
```

## Layout

| Path | Purpose |
| ---- | ------- |
| `firmware/main/main.c` | Entry point: sampling loop and CSV output |
| `firmware/main/hx711.c`, `hx711.h` | HX711 driver |
| `firmware/sdkconfig` | ESP-IDF configuration (target `esp32c3`) |
| `firmware/build/` | Generated build output; `idf.py build` recreates it |

## Next

Phase 1 only streams raw counts. The next steps are calibrating counts against
known loads, filtering, and turning bridge readings into a posture signal.
