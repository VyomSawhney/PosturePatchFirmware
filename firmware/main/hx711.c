#include "hx711.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"

void hx711_init(hx711_t *hx, int dout, int sck) {
    hx->dout = dout;
    hx->sck  = sck;
    gpio_config_t in_cfg = { .pin_bit_mask = 1ULL << hx->dout, .mode = GPIO_MODE_INPUT, .pull_up_en = 0, .pull_down_en = 0, .intr_type = GPIO_INTR_DISABLE };
    gpio_config(&in_cfg);
    gpio_config_t out_cfg = { .pin_bit_mask = 1ULL << hx->sck, .mode = GPIO_MODE_OUTPUT, .pull_up_en = 0, .pull_down_en = 0, .intr_type = GPIO_INTR_DISABLE };
    gpio_config(&out_cfg);
    gpio_set_level(hx->sck, 0);
}

bool hx711_is_ready(hx711_t *hx) {
    return gpio_get_level(hx->dout) == 0; // DOUT low means data ready
}

bool hx711_read(hx711_t *hx, int32_t *value) {
    if (!hx711_is_ready(hx)) return false;
    int32_t data = 0;
    for (int i = 0; i < 24; i++) {
        gpio_set_level(hx->sck, 1);
        esp_rom_delay_us(1);
        data = (data << 1) | gpio_get_level(hx->dout);
        gpio_set_level(hx->sck, 0);
        esp_rom_delay_us(1);
    }
    // One extra clock to set gain=128 (channel A)
    gpio_set_level(hx->sck, 1);
    esp_rom_delay_us(1);
    gpio_set_level(hx->sck, 0);

    // Sign-extend 24-bit two's complement
    if (data & 0x800000) data |= ~0xFFFFFF;
    *value = data;
    return true;
}
