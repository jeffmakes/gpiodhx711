#ifndef __HX711_H
#define __HX711_H

#include <gpiod.h>
#include <stdint.h>
#include <unistd.h>

typedef struct hx711_handle_t 
{
    struct gpiod_chip *chip;
	struct gpiod_line *pd_sck;
	struct gpiod_line *dout;
	struct gpiod_line *en;
    int32_t gain;
	int32_t offset;
	float scale;
} hx711_handle_t;


hx711_handle_t* hx711_init(uint32_t gpio_pd_sck, uint32_t gpio_dout, uint32_t gpio_en);
void hx711_deinit(hx711_handle_t* hx);


bool hx711_isready(hx711_handle_t* hx);
int32_t hx711_read_raw(hx711_handle_t* hx);
int32_t hx711_read_average(hx711_handle_t* hx);
float hx711_read_kg(hx711_handle_t* hx);

void hx711_set_gain(hx711_handle_t* hx, uint8_t gain);
void hx711_set_scale(hx711_handle_t* hx, float scale);
void hx711_tare(hx711_handle_t* hx);
void hx711_power(hx711_handle_t* hx, bool state);

#endif // __HX711_H 
