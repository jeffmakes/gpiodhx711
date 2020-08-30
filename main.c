#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif

typedef struct hx711_handle_t 
{
	struct gpiod_chip *chip;
	struct gpiod_line *pd_sck;
	struct gpiod_line *dout;
	int32_t gain;
	uint32_t offset;
	uint32_t scale;
} hx711_handle_t;

hx711_handle_t* hx711_init(uint32_t gpio_pd_sck, uint32_t gpio_dout) 
{
	int32_t req;
	char* chipname = "gpiochip0";

	hx711_handle_t* handle = malloc(sizeof(hx711_handle_t));
	if (!handle){
		perror("malloc failed");
		goto end;
	}
	printf("got to here");

	handle->chip = gpiod_chip_open_by_name(chipname);
	if (!handle->chip) {
		perror("Open chip failed\n");
		goto end;
	}

	handle->pd_sck = gpiod_chip_get_line(handle->chip, gpio_pd_sck);
	if (!handle->pd_sck) {
		perror("Get clock line failed\n");
		goto close_chip;
	}

	handle->dout = gpiod_chip_get_line(handle->chip, gpio_dout);
	if (!handle->dout) {
		perror("Get data line failed\n");
		goto close_chip;
	}

	req = gpiod_line_request_output(handle->pd_sck, CONSUMER, 0);
	if (req < 0) {
		perror("Request clock line as output failed\n");
		goto release_line;
		}
	req = gpiod_line_request_input(handle->dout, CONSUMER);
	if (req < 0) {
		perror("Request data line as input failed\n");
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

bool hx711_isready(hx711_handle_t* hx)
{
	return (gpiod_line_get_value(hx->dout) == 0);
}

int32_t hx711_read(hx711_handle_t* hx)
{
	uint32_t i;
	uint32_t bits = 0;

	while ( !hx711_isready(hx) );

	for (i = 0; i < 24; i++)
	{
		gpiod_line_set_value(hx->pd_sck, 1);
		bits |= gpiod_line_get_value(hx->dout) << i;
		gpiod_line_set_value(hx->pd_sck, 0);
	}
       	//set channel and gain factor for next reading
	for (i = 0; i < hx->gain; i++)
	{
		gpiod_line_set_value(hx->pd_sck, 1);
		gpiod_line_set_value(hx->pd_sck, 0);
	}
	//pad out to 32 bits 2's complement
	//... figure this out later... see python driver
	return bits;	
}


int main(int argc, char **argv)
{
	char *chipname = "gpiochip0";
	unsigned int line_num = 12;	// GPIO Pin number for 3v3_EN
	unsigned int val;
	struct gpiod_chip *chip;
	struct gpiod_line *line;
	int i, ret;

	//turn on 3v3 rail
	chip = gpiod_chip_open_by_name(chipname);
	line= gpiod_chip_get_line(chip, line_num);
	gpiod_line_request_output(line, CONSUMER, 0);
	gpiod_line_set_value(line, 1);


	printf("got to here");
	hx711_handle_t* scale0 = hx711_init(4, 14);
	//hx711_handle_t* scale1 = hx711_init(15, 17);
	
	for (i = 0; i<60; i++)
	{
		printf("%d\n", hx711_read(scale0));
		sleep(1);
	}
	
	hx711_deinit(scale0);
	//hx711_deinit(scale1);

	return 0;

	/* Blink 20 times */
/*	val = 0;
	for (;;) {
		ret = gpiod_line_set_value(line, val);
		if (ret < 0) {
			perror("Set line output failed\n");
			goto release_line;
		}
		//printf("Output %u on line #%u\n", val, line_num);
		//sleep(1);
		val = !val;
	}
*/
}
