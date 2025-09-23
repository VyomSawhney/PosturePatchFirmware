#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int dout;
    int sck;
} hx711_t;

void hx711_init(hx711_t *hx, int dout, int sck);
bool hx711_is_ready(hx711_t *hx);
bool hx711_read(hx711_t *hx, int32_t *value);
