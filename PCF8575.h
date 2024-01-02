////////////////////////////////////////////////////////////////////////////////////////////////
//																				              //
//     FILE:  PCF8575.h																	      //
//   AUTHOR:  Eric Dawe								                        	              //
//     DATE:  02-01-2024 (DD-MM-YYYY)													      //
//  VERSION:  0.1.0																		      //
//    TITLE:  An RP2040 library for PCF8575 I/O expander modules							  //
// LANGUAGE:  C++																		      //
//      URL:  https://github.com/redocrehtona/PCF8575.h							              //
//																				              //
//																				              //
// License:																		              //
// ------------------------------------------------------------------------------------------ //
//																						      //
// MIT License																			      //
//																						      //
// Copyright (c) 2024 Eric Dawe															      //
//																						      //
// Permission is hereby granted, free of charge, to any person obtaining a copy			      //
// of this software and associated documentation files (the "Software"), to deal		      //
// in the Software without restriction, including without limitation the rights			      //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell			      //
// copies of the Software, and to permit persons to whom the Software is				      //
// furnished to do so, subject to the following conditions:				 				      //
//																						      //
// The above copyright notice and this permission notice shall be included in all		      //
// copies or substantial portions of the Software.										      //
//																						      //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR			      //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,				      //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE			      //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER				      //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,		      //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE		      //
// SOFTWARE.																			      //
//																						      //
//																						      //
// Description:																			      //
// ------------------------------------------------------------------------------------------ //
//																						      //
// A simple single-file c++ library written for the Raspberry Pi Pico (or any RP2040 board)   //
// to communicate with I/O expander modules based on the PCF8575 microchip.                   //
// 																						      //
//																						      //
// Use:																					      //
// ------------------------------------------------------------------------------------------ //
//                                                                                            //
// To initialise a new expander:														      //
//	PCF8575 expander_name(sda_pin, scl_pin, i2c_bus, expander_address);					      //
// 																						      //
// To set the state of one or many pins:												      //
//	expander_name.write(state_bool, first_pin_int, pin_count_int);						      //
//																						      //
// To read the state of all pins (returns an array of 16 boolean values)				      //
//	expander_name.read();																      //
//																						      //
// The state of just one pin can be selected from the array after it is returned:		      //
//	bool pin_seven = expander_name.read()[7];											      //
//																						      //
//																						      //
// For more information see README.md at https://github.com/redocrehtona/PCF8575.h		      //
//																						      //
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PCF8575_H
#define PCF8575_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

class PCF8575{
	private:
	i2c_inst_t *I2C;
	uint8_t I2C_ADDR;
	uint16_t data = 0x0000;

	public:
	PCF8575(int sda_pin, int scl_pin, i2c_inst_t *i2c_bus, uint8_t expander_i2c_address) {
		I2C = i2c_bus;
		I2C_ADDR = expander_i2c_address;
	
		i2c_init(I2C, 400 * 1000);
	
		gpio_set_function(sda_pin, GPIO_FUNC_I2C);
		gpio_set_function(scl_pin, GPIO_FUNC_I2C);

	}

	void write(bool state, int first_pin, int pin_count) {
		// If we are setting the bit(s)
		if (state) {
			for (int i = 0; i < pin_count && i + first_pin < 16; i++) {
				data |= ((uint8_t)1 << first_pin + i);
			}
		}
		// Else we are clearing the bit(s)
		else {
			for (int i = 0; i < pin_count && i + first_pin < 16; i++) {
				data &= ~((uint8_t)1 << first_pin + i);
			}
		}

		uint8_t msg[2] = { 0x00, 0x00 };

		msg[1] = (uint8_t)((data & 0xff00) >> 8); msg[0] = (uint8_t)(data & 0x00ff);

		i2c_write_blocking(I2C, I2C_ADDR, msg, 2, true);
	}

	bool *read() {
		static bool read_values[16] = {
			0, 0, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 0
		};

		uint8_t read[2] = { 0x00, 0x00 };

		i2c_read_blocking(I2C, I2C_ADDR, read, 2, true);

		// Copy read values to array
		// First 8-bit integer (pins 0 to 7)
		for ( int i = 0; i < 8; i++ ) {
			read_values[i] = ( ( read[0] & (1 << i) ) ? 1 : 0 );
		}
		// Second 8-bit integer (pins 8 to 15)
		for ( int i = 0; i < 8; i++ ) {
			read_values[i + 8] = ( ( read[1] & (1 << i) ) ? 1 : 0 );
		}


		return read_values;
	};
};

#endif
