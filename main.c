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


int main(int argc, char **argv)
{
//	char *chipname = "gpiochip0";
	unsigned int line_num = 7;	// GPIO Pin number 
	unsigned int val;
	struct gpiod_chip *chip;
	struct gpiod_line *line;
	int i, ret;

	printf("got to here");
	hx711_handle_t* scale0 = hx711_init(4, 14);
	//hx711_handle_t* scale1 = hx711_init(15, 17);
	
	printf("%d\n", scale0->gain);
	
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
