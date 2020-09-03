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
	int32_t offset;
	float scale;
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


	hx711_handle_t* scale0 = hx711_init(4, 14);
	hx711_handle_t* scale1 = hx711_init(15, 17);
	
	printf("%d\n", hx711_read_average(scale0));
	printf("%d\n", hx711_read_average(scale1));
	hx711_tare(scale0);
	hx711_tare(scale1);
	hx711_set_scale(scale0, 0.0000184267); //assuming Jeff as 100kg proof mass
	hx711_set_scale(scale1, 0.0000184267); //assuming Jeff as 100kg proof mass
	for (i = 0; i<600; i++)
	{
		printf("%d\t%f\t%f\n", i, hx711_read_kg(scale0),hx711_read_kg(scale1) );
		sleep(0.1);
	}
	
	hx711_deinit(scale0);
	//hx711_deinit(scale1);

	return 0;
}
