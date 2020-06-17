/*
	c-hdc1080 is a C library for the Texas Instruments HDC1080
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

#include "hdc1080.h"

enum hdc1080_status hdc1080_init(struct hdc1080_t *device, void *ctx)
{
	device->ctx = ctx;
	enum hdc1080_status status = _hdc1080_init(device);
	if(status == hdc1080_status_ok)
	{
		status = hdc1080_get_configuration(device);
	}
	return status;
}

enum hdc1080_status hdc1080_set_configuration(struct hdc1080_t *device)
{
	enum hdc1080_status status = _hdc1080_write(device, hdc1080_command_configuration, sizeof(device->configuration), (uint8_t*)&device->configuration);
	device->configuration.value = LE16(device->configuration.value);
	return status;
}

enum hdc1080_status hdc1080_get_configuration(struct hdc1080_t *device)
{
	enum hdc1080_status status = _hdc1080_read(device, hdc1080_command_configuration, 0, sizeof(device->configuration), (uint8_t*)&device->configuration);
	device->configuration.value = LE16(device->configuration.value);
	return status;
}

enum hdc1080_status hdc1080_get_reading(struct hdc1080_t *device, double *temperature, double *humidity)
{
	uint8_t buf[4];
	unsigned int th, tl, hh, hl;
	
	int delay = htc1080_get_temperature_conversion_time(device->configuration.fields.TRES) +
	htc1080_get_humidity_conversion_time(device->configuration.fields.HRES);
	
	enum hdc1080_status result = _hdc1080_read(device, hdc1080_command_temperature, delay, sizeof(buf), buf);	

	if(result == hdc1080_status_ok)
	{
		th = buf[0];
		tl = buf[1];
		hh = buf[2];
		hl = buf[3];

		*temperature = (th << 8 | tl) * 165.0 / 65536.0 - 40.0;
		*humidity = (hh << 8 | hl) * 100.0 / 65536.0;
	}
	return result;
}

enum hdc1080_status hdc1080_get_manufacturer_id(struct hdc1080_t *device, uint16_t *manufacturer_id)
{
	uint16_t tmp;
	enum hdc1080_status status = _hdc1080_read(device, hdc1080_command_manufacturer_id, 0, sizeof(tmp), (uint8_t*)&tmp);
	*manufacturer_id = BE16(tmp);
	return status;
}

enum hdc1080_status hdc1080_get_device_id(struct hdc1080_t *device, uint16_t *device_id)
{
	uint16_t tmp;
	enum hdc1080_status status = _hdc1080_read(device, hdc1080_command_device_id, 0, sizeof(tmp), (uint8_t*)&tmp);
	*device_id = BE16(tmp);
	return status;
}

double htc1080_get_temperature_conversion_time(enum hdc1080_humidity_resolution_e resolution)
{
	switch(resolution)	
	{
		//Page 5 of datasheet
		case hdc1080_humidity_resolution_8_bits:
			return 3; //2.5ms
		break;
		case hdc1080_humidity_resolution_11_bits:
			return 4; //3.85ms
		break;
		case hdc1080_humidity_resolution_14_bits:
			return 7; //6.5ms
		break;
	}
	
	return 7; //6.5ms
};
	
double htc1080_get_humidity_conversion_time(enum hdc1080_temperature_resolution_e resolution)
{
	switch(resolution)
	{
		//Page 5 of datasheet
		case hdc1080_temperature_resolution_11_bits:
			return 4; //3.65ms
			break;
		case hdc1080_temperature_resolution_14_bits:
			return 7; //6.35ms
			break;
	}
	return 7; //6.5ms
};

__attribute__((weak)) enum hdc1080_status _hdc1080_init(struct hdc1080_t *device)
{
	return hdc1080_status_ok;
};

__attribute__((weak)) enum hdc1080_status _hdc1080_write(struct hdc1080_t *device, enum hdc1080_command_e command, size_t count, uint8_t *buf)
{
	return hdc1080_status_ok;
};

__attribute__((weak)) enum hdc1080_status _hdc1080_read(struct hdc1080_t *device, enum hdc1080_command_e command, uint16_t delayms, size_t count, uint8_t *buf)
{
	return hdc1080_status_ok;
};