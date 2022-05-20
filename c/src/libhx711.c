#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "../include/libhx711.h"

    
hx711_handle_t* hx711_init(uint32_t gpio_pd_sck, uint32_t gpio_dout) 
{
	int32_t req;
	char* chipname = CHIPNAME;

	hx711_handle_t* handle = malloc(sizeof(hx711_handle_t));
	if (!handle){
		perror("malloc failed");
		goto end;
	}

	handle->chip = gpiod_chip_open_by_name(chipname);
	if (!handle->chip) {
		perror("Open chip failed");
		goto end;
	}

    // Get GPIO lines
	handle->pd_sck = gpiod_chip_get_line(handle->chip, gpio_pd_sck);
	if (!handle->pd_sck) {
		perror("Get clock line failed");
		goto close_chip;
	}

	handle->dout = gpiod_chip_get_line(handle->chip, gpio_dout);
	if (!handle->dout) {
		perror("Get data line failed");
		goto close_chip;
	}
	
    printf("sck %d   dout %d\n", gpio_pd_sck, gpio_dout);
    // Set GPIO line directions
	req = gpiod_line_request_output(handle->pd_sck, CONSUMER, 0);
	if (req < 0) {
		perror("Request clock line as output failed");
		goto release_line;
		}

	req = gpiod_line_request_input(handle->dout, CONSUMER);
	if (req < 0) {
		perror("Request data line as input failed");
		goto release_line;
		}
    
	//set defaults
	handle->gain = 1; // set default gain to 128
	handle->offset = 0;
	handle->scale = 1;	
	
	return handle;

release_line:
	gpiod_line_release(handle->pd_sck);
	gpiod_line_release(handle->dout);
close_chip:
	gpiod_chip_close(handle->chip);
end:
	return NULL;
}

void hx711_deinit(hx711_handle_t* hx)
{
	gpiod_line_release(hx->pd_sck);
	gpiod_line_release(hx->dout);
	gpiod_chip_close(hx->chip);
	free(hx);
}

struct gpiod_line *power_en = NULL;
struct gpiod_chip *power_chip = NULL;
bool hx711_power_init()
{
    char* chipname = CHIPNAME;
    if (power_en != NULL)       // Don't initialise if we've already done so.
        return false;

	power_chip = gpiod_chip_open_by_name(CHIPNAME);
	if (!power_chip) {
		perror("Power init: open chip failed");
		goto end;
	}

    // Get GPIO lines
	power_en = gpiod_chip_get_line(power_chip, PIN_3V3EN);
	if (!power_en) {
		perror("Power init: get en line failed");
		goto end;
	}
	if (gpiod_line_request_output(power_en, CONSUMER, 0) < 0){
		perror("Request en line as output failed");
		goto release_line;
	}
    return false;
release_line:
	gpiod_line_release(power_en);
end: 
    return true;
}

void hx711_set_power(bool state)
{
    if (gpiod_line_set_value(power_en, state)){
        perror("Failed to set power EN line");
    }
}

bool hx711_isready(hx711_handle_t* hx)
{
	return (gpiod_line_get_value(hx->dout) == 0);
}

void printbin(uint32_t p)
{	
	for (int i = 31 ; i>-1; i--)
	{
		if(p & (1<<i))
			printf("1");	
		else
			printf("0");
	}
	printf("\n");
}

int32_t hx711_read_raw(hx711_handle_t* hx)
{
	uint32_t i;
	int32_t val = 0;

	while ( !hx711_isready(hx) ); //TODO: add a timeout?

	//clock out data
	for (i = 0; i < 24; i++)
	{
		gpiod_line_set_value(hx->pd_sck, 1);
		val |= gpiod_line_get_value(hx->dout); 
		val <<= 1;
		gpiod_line_set_value(hx->pd_sck, 0);
	}
       	
	//set channel and gain factor for next reading
	for (i = 0; i < hx->gain; i++)
	{
		gpiod_line_set_value(hx->pd_sck, 1);
		gpiod_line_set_value(hx->pd_sck, 0);
	}
	
	// pad to 32-bit 2's complement
	if (val & (1<<23))
	{
		val |= 0xff000000;
	}
	return val;
}

#define NUM_AVERAGES 10 
int32_t hx711_read_average(hx711_handle_t* hx)
{
	uint32_t times;
	int32_t res = 0;

    hx711_read_raw(hx);     //First value read after init is often junk. Dump a few values before computing average
    hx711_read_raw(hx);
    hx711_read_raw(hx);

	times = NUM_AVERAGES;
	while(times--)
	{
		res += hx711_read_raw(hx);
	}
	res /= NUM_AVERAGES;
 	return res;
}

void hx711_set_gain(hx711_handle_t* hx, uint8_t gain)
{
	if (gain == 128)
		hx->gain = 1;
	if (gain == 64)
		hx->gain = 3;
	if (gain == 32)
		hx->gain = 2;
}

void hx711_tare(hx711_handle_t* hx)
{
	int32_t val;

	val = hx711_read_average(hx);
	hx->offset = val;
}

void hx711_set_scale(hx711_handle_t* hx, float scale)
{
	hx->scale = scale;
}

float hx711_read_kg(hx711_handle_t* hx)
{
	return (hx711_read_raw(hx) - hx->offset) * hx->scale;
}

