Building
--------

Install C dependencies:

For building libgpiodhx711.a:
libgpiod2
libgpiod-dev
gpiod

For building the python extension:
libpython3.9-dev
python-dev

Build the C library and test program. This creates the libgpiodhx711.a archive:
cd gpiodhx711/c
make

Python bindings:

0) Create a new venv:
cd gpiod/python
python3 -m venv venv

1) Activate the virtual environment:
source venv/bin/activate

2) On RPiOS I had to install some surprisingly missing python dependencies:
pip install setuptools wheel

3) Build and install the extension. This creates the _gpiodhx711.c file and compiles it, along with libgpiodhx711.a, and links in the gpiod library from the system. The output is 
_gpiodhx711.abi3.so: 

python setup.py develop
or 
python setup.py install


4) Run the module 

python gpiodhx711.py






