from setuptools import setup, find_packages

setup(name="gpiodhx711",
      version="0.1",
      description="Fast driver for the HX711 load cell amplifier based on the Kernel gpiod interface, including Python bindings",
      author="Jeff Gough",
      py_modules=["gpiodhx711"],
      setup_requires=["cffi", "path"],
      cffi_modules=["build_gpiodhx711.py:ffibuilder"],
      install_requires=["cffi"],
)
