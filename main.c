#include <stdio.h>
#include <unistd.h>
#include "hx711.h"

#define PIN_3V3EN 12

int main(int argc, char **argv)
{
	char *chipname = "gpiochip0";
	unsigned int i;

	hx711_handle_t* scale0 = hx711_init(4, 14, PIN_3V3EN);
	hx711_handle_t* scale1 = hx711_init(15, 17, PIN_3V3EN);
    hx711_power(scale0, true);

	//printf("%d\n", hx711_read_average(scale0));
	//printf("%d\n", hx711_read_average(scale1));
	hx711_tare(scale0);
	hx711_tare(scale1);
	hx711_set_scale(scale0, 0.0000184267); //assuming Jeff as 100kg proof mass
	hx711_set_scale(scale1, 0.0000184267); //assuming Jeff as 100kg proof mass
	for (i = 0; i<60000; i++)
	{
		printf("%d\t%f\t%f\n", i, hx711_read_kg(scale0),hx711_read_kg(scale1) );
	}
	
	hx711_deinit(scale0);
	//hx711_deinit(scale1);

	return 0;
}

