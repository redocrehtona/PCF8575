# PCF8575.h

A Raspberry Pi Pico library for PCF8575 I/O expander modules.

## Description

A simple single-file c++ library written for the Raspberry Pi Pico (or any RP2040 board) to communicate with I/O expander modules based on the PCF8575 microchip.  

## Requirements

#### The Raspberry Pi Pico SDK
This library can only be used alongside the Raspberry Pi Pico SDK.

#### Enable I²C

I²C must be enabled by adding `hardware_i2c` to the list of linked libraries in your CMakeLists.txt, e.g.:
```
target_link_libraries(your_project pico_stdlib ... hardware_i2c ...)
```

#### Include `PCF8575.h`

`PCF8575.h` must be included in the source file where it is to be used.

## Use

### Initialisation

This library provides the class `PCF8575`.  Each expander must first be initialised by creating an instance of this class with the correct parameters.  

Initialising a new expander looks something like this:  
```
PCF8575 expander_name(sda_pin, scl_pin, i2c_bus, expander_address);
```
For example, initialising an expander called `myexpander` with the default address `0x20` connected to the I²C bus `i2c0` on pins 20 and 21 of a Raspberry Pi Pico would look like this:  
```
PCF8575 myexpander(20, 21, i2c0, 0x20);
```


### Setting/Writing

To set the state of one or many pins:
```
expander_name.write(state_bool, first_pin_int, pin_count_int);		
```
`state_bool` must be either `0` (low) or `1` (high).  
`first_pin_int` is the first (or only) pin you wish to set.  Pins are numbered from 0 to 15  
`pin_count_int` is the number of pins, counting upwards, to be set to this state.  Use `1` to set only `first_pin`  

For example, to set all 16 pins on `myexpander` to `1` (high): 
```
myexpander.write(1, 0, 16);
```

To then set only pin 7 to `0` (low):
```
myexpander.write(0, 7, 1);
```


### Reading


The `read()` function returns an array of 16 boolean values:  
```
expander_name.read()
```

Individual pins can be selected by appending `[pin_number]` to the `read()` call.  

For example, to initialise an array of 16 booleans and then set them to the current state of the I/O expander every time the `read()` function is called:  
```
// When initialising variables
bool[16] states = {0};

// Later, e.g. in a loop or on an input call
states = myexpander.read();
```

Or, to get just one value as needed:  
```
bool pin_seven = 0;

pin_seven = myexpander.read()[7];
```

