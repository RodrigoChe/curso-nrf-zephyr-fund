#ifndef TEMPERATURE_SENSOR_SERVICE_H
#define TEMPERATURE_SENSOR_SERVICE_H

#include <zephyr/bluetooth/bluetooth.h>

// Service UUID
#define TEMPERATURE_SENSOR_SERVICE_UUID BT_UUID_128_ENCODE(0xf5fb5c9f, 0x4487, 0x4bdc, 0xa5de, 0x057dcc899273)

// Temp Data characteristic
#define TEMP_DATA_CHAR_UUID BT_UUID_128_ENCODE(0xc81bac9d, 0x4c6d, 0x4116, 0xb347, 0xae7f3e837576)

// Sampling Interval characteristic
#define SAMPLING_INTERVAL_CHAR_UUID BT_UUID_128_ENCODE(0x78574b27, 0xf07f, 0x43ec, 0xa1a8, 0x8f0fcebf662f)

int temperature_sensor_service_init(void);
int send_temp_data_notification(int16_t temp_data);

#endif /* TEMPERATURE_SENSOR_SERVICE_H */