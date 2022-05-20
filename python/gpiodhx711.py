from _gpiodhx711 import lib, ffi


class GpiodHx711:
    def __init__(self):
        self._hx = lib.hx711_init(gpio_sck, gpio_dout)
    
    def __del__(self):
        lib.hx711_deinit(self._hx)


def main():
    scale0 = GpiodHx711(4, 14)
    
if __name__ == "__main__":
    main()

        
