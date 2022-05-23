from _gpiodhx711 import lib, ffi


class GpiodHx711:
    def __init__(self, gpio_sck, gpio_dout, gpio_power_en):
        self._hx = lib.hx711_init(gpio_sck, gpio_dout, gpio_power_en)
    
    def deinit(self):
        lib.hx711_deinit(self._hx)

    def set_power(self, value):
        lib.hx711_set_power(self._hx, value)

    def tare(self):
        lib.hx711_tare(self._hx)

    def set_scale(self, scale):
        lib.hx711_set_scale(self._hx, scale)

    def read_kg(self):
        return lib.hx711_read_kg(self._hx)


def main():
    try:
        scale0 = GpiodHx711(4, 14, 12)
        scale1 = GpiodHx711(15, 17, 12)

        scale0.set_power(True)
        scale1.set_power(True)

        scale0.tare()
        scale0.set_scale(0.0000184267) #assuming Jeff as 100kg proof mass
        scale1.tare()
        scale1.set_scale(0.0000184267) #assuming Jeff as 100kg proof mass

        for i in range (500):
            kg = scale0.read_kg()
            print("Scale 0: {:.2f} kg   Scale 1: {:.2f} kg".format(scale0.read_kg(), scale1.read_kg()))

    except:
        print("Something broke")
    finally:
        print("Clearing up...")
        scale0.deinit()
        scale1.deinit()



if __name__ == "__main__":
    main()

        
