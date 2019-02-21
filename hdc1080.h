/*
	c-hdc1080 is a C library to use the Texas Instruments HDC1080
	High Accuracy Digital Humidity Sensor with Temperature Sensor.
	Copyright (C) 2019 NigelB

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef HDC1080_H_
#define HDC1080_H_

#include "stddef.h"
#include "stdint.h"
#include "compiler.h"

#define HDC1080_ADDRESS            0x40


#define hdc1080_swap16(_value) ((uint16_t)(((uint16_t)(_value) >> 8) | ((uint16_t)(_value) << 8)))
#define BE16(x)        hdc1080_swap16(x)
#define LE16(x)        (x)

enum hdc1080_command_e
{
		hdc1080_command_temperature     = 0x00,
		hdc1080_command_humidity        = 0x01,
		hdc1080_command_configuration   = 0x02,
		hdc1080_command_serial_id       = 0xFB,
		hdc1080_command_manufacturer_id = 0xFE,
		hdc1080_command_device_id       = 0xFF
};

enum hdc1080_humidity_resolution_e
{
	hdc1080_humidity_resolution_14_bits = 0b00,
	hdc1080_humidity_resolution_11_bits = 0b01,
	hdc1080_humidity_resolution_8_bits  = 0b10
};

enum hdc1080_temperature_resolution_e
{
	hdc1080_temperature_resolution_14_bits = 0,
	hdc1080_temperature_resolution_11_bits = 1
};

enum hdc1080_battery_status_e
{
		hdc1080_battery_status_good = 0,
		hdc1080_battery_status_low = 1
};

enum hdc1080_acquisition_mode_e
{
	//When the device is set to acquire, it will take a Temperature then a Humidity and then go back to sleep.
	hdc1080_acquisition_mode_sleep = 0,
	hdc1080_acquisition_mode_acquire = 1,
};

enum hdc1080_heater_e
{
		hdc1080_heater_disabled = 0,
		hdc1080_heater_enabled  = 1
};

enum hdc1080_reset_e
{
		hdc1080_reset_normal_operation = 0,
		hdc1080_reset_software_reset = 1
};

enum hdc1080_ids_e
{
	hdc1080_manufacturer_id	= 0x5449,
	hdc1080_device_id       = 0x1050	
};

struct hdc1080_configuration_t
{
	union{
			uint16_t value;
			struct{
					uint8_t RESERVED_1: 8;
					uint8_t HRES: 2;
					uint8_t TRES: 1;
					uint8_t BTST: 1;
					uint8_t MODE: 1;
					uint8_t HEAT: 1;
					uint8_t RESERVED: 1;
					uint8_t RSTL: 1;
				}fields;
		};	
};

struct hdc1080_t
{
	struct hdc1080_configuration_t configuration;
	void *ctx;
};

enum hdc1080_status{
	hdc1080_status_undefined,
	hdc1080_status_ok
};

enum hdc1080_status hdc1080_init(struct hdc1080_t *device, void *ctx);
enum hdc1080_status hdc1080_set_configuration(struct hdc1080_t *device);
enum hdc1080_status hdc1080_get_configuration(struct hdc1080_t *device);

enum hdc1080_status hdc1080_get_reading(struct hdc1080_t *device, double *temperature, double *humidity);
enum hdc1080_status hdc1080_get_manufacturer_id(struct hdc1080_t *device, uint16_t *manufacturer_id);
enum hdc1080_status hdc1080_get_device_id(struct hdc1080_t *device, uint16_t *device_id);

double htc1080_get_temperature_conversion_time(enum hdc1080_humidity_resolution_e resolution);
double htc1080_get_humidity_conversion_time(enum hdc1080_temperature_resolution_e resolution);

enum hdc1080_status _hdc1080_init(struct hdc1080_t *device);
enum hdc1080_status _hdc1080_read(struct hdc1080_t *device, enum hdc1080_command_e command, uint16_t delayms, size_t count, uint8_t *buf);
enum hdc1080_status _hdc1080_write(struct hdc1080_t *device, enum hdc1080_command_e command, size_t count, uint8_t *buf);

#endif