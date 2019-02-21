# c-hdc1080
A C library for the Texas Instruments HDC1080 High Accuracy Digital Humidity Sensor with Temperature Sensor.

Usage
=====
To use this library you will need to implement 3 hardware specific functions:

  * `_hdc1080_init` - Do any required hardware setup. N.B. the ctx pointer passed to the `hdc1080_init` function is available via the `device->ctx` pointer. 
  * `_hdc1080_read` - Implement the platform specific I2C read from the HDC1080.
  * `_hdc1080_write`- Implement the platform specific I2C write to the HDC1080.
  
This is an example for as ASF4 project:
```C
enum hdc1080_status _hdc1080_init(struct hdc1080_t *device)
{
        enum hdc1080_status status = hdc1080_status_ok;
        int32_t result = i2c_m_sync_set_slaveaddr((struct i2c_m_sync_desc *)device->ctx, HDC1080_ADDRESS, I2C_M_SEVEN);
        return status;  
};


enum hdc1080_status _hdc1080_read(struct hdc1080_t *device, enum hdc1080_command_e command, uint16_t delayms, size_t count, uint8_t *buf)
{
        enum hdc1080_status status = hdc1080_status_ok;
        struct i2c_m_sync_desc * i2c = (struct i2c_m_sync_desc*)device->ctx;
        struct io_descriptor *I2C_0_io;

        i2c_m_sync_get_io_descriptor(i2c, &I2C_0_io);   
        
        io_write(I2C_0_io, &command, 1);        
        if(delayms > 0){delay_ms(delayms);}
        io_read(I2C_0_io, buf, count);
        return status;  
};

enum hdc1080_status _hdc1080_write(struct hdc1080_t *device, enum hdc1080_command_e command, size_t count, uint8_t *buf)
{
        enum hdc1080_status status = hdc1080_status_ok;
        uint8_t write_buffer[1 + count];
        write_buffer[0] = command;
        memcpy(&write_buffer[1], buf, count);

        struct i2c_m_sync_desc * i2c = (struct i2c_m_sync_desc*)device->ctx;
        struct io_descriptor *I2C_0_io;

        i2c_m_sync_get_io_descriptor(i2c, &I2C_0_io);

        io_write(I2C_0_io, write_buffer, sizeof(write_buffer));
        return status;
};
```


Which can then be used to make a reading:
```C
bool read_hdc1080(double* temperature, double* humidity)
{
        struct hdc1080_t hdc1080;
        hdc1080_init(&hdc1080, &I2C_0);
        
        uint16_t manufacture_id, device_id;
        
        hdc1080_get_manufacturer_id(&hdc1080, &manufacture_id);
        hdc1080_get_device_id(&hdc1080, &device_id);
        
        hdc1080.configuration.fields.HRES = hdc1080_humidity_resolution_14_bits;
        hdc1080.configuration.fields.TRES = hdc1080_temperature_resolution_14_bits;
        hdc1080.configuration.fields.MODE = hdc1080_acquisition_mode_acquire;
        
        printf("Manufacturer ID: 0x%04x\r\n", manufacture_id);
        printf("Device ID      : 0x%04x\r\n", device_id);       
        
        hdc1080_set_configuration(&hdc1080);
        
        hdc1080_get_reading(&hdc1080, temperature, humidity);
        printf("Temperature: %f, Humidity: %f\r\n", *temperature, *humidity);
        
        return (manufacture_id == hdc1080_manufacturer_id) && (device_id == hdc1080_device_id); 
}
```
