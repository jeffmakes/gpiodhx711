Building
--------

C library and test program. This creates the libgpiodhx711.a archive.

cd gpiodhx711/c
make

Python bindings:

1) Activate the virtual environment:
cd gpiod/python
source venv/bin/activate

2) Build the extension. This creates the _gpiodhx711.c file and compiles it, along with libgpiodhx711.a, and links in the gpiod library from the system. The output is 
_gpiodhx711.abi3.so: 

python setup.py build_ext

3) Not exactly sure what the next step does - it creates a copy of _gpiodhx711.abi3.so from the build folder to the root. It also creates gpiodhx711.egg-info folder. So I guess it's building the python project/package/egg/wheel (Not sure which, nor how they differ):

python setup.py develop
and/or? 
python setup.py install


4) Run the module 

python gpiodhx711.py






