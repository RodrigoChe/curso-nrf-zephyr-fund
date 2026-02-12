#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(aht_lsm6dsl_driver, LOG_LEVEL_INF);

#define SENSOR_NODE DT_NODELABEL(lsm6ds3tr_c)

int main(void)
{
    const struct device *dev = DEVICE_DT_GET(SENSOR_NODE);
    struct sensor_value odr_attr;

    if (!device_is_ready(dev)) {
        LOG_ERR("Sensor device not ready");
        return -1;
    }

	/* set accel/gyro sampling frequency to 104 Hz */
	odr_attr.val1 = 104;
	odr_attr.val2 = 0;

	if (sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ,
			    SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
		printk("Cannot set sampling frequency for accelerometer.\n");
		return 0;
	}

	if (sensor_attr_set(dev, SENSOR_CHAN_GYRO_XYZ,
			    SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
		printk("Cannot set sampling frequency for gyro.\n");
		return 0;
	}

	if (sensor_sample_fetch(dev) < 0) {
		printk("Sensor sample update error\n");
		return 0;
	}

    //const struct device *aht0 = DEVICE_DT_GET_ONE(renesas_hs300x);
    const struct device *aht0 = DEVICE_DT_GET_ONE(aosong_dht20);

    if (!device_is_ready(aht0)) {
        LOG_ERR("aht0 device not ready");
        return -1;
    }

    LOG_INF("AHT and lsm6dsl sensors drivers ready");

    while (1) {
        struct sensor_value accel[3];
        struct sensor_value temp, hum;

        if (sensor_sample_fetch(dev) < 0) {
            LOG_ERR("Failed to fetch sensor data");
            return -1;
        }

        if (sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel) < 0) {
            LOG_ERR("Failed to get sensor channel data");
            return -1;
        }

        LOG_INF("Accel [m/s^2]: X=%.3f Y=%.3f Z=%.3f",
                sensor_value_to_double(&accel[0]),
                sensor_value_to_double(&accel[1]),
                sensor_value_to_double(&accel[2]));

        if (sensor_sample_fetch(aht0) < 0) {
            LOG_ERR("Failed to fetch sample");
            return -1;
        }

        if (sensor_channel_get(aht0, SENSOR_CHAN_HUMIDITY, &hum) < 0) {
            LOG_ERR("Failed to get humidity");
            return -1;
        }        
        if (sensor_channel_get(aht0, SENSOR_CHAN_AMBIENT_TEMP, &temp) < 0) {
            LOG_ERR("Failed to get temperature");
            return -1;
        }
        LOG_INF("Temperature: %0.3f °C", sensor_value_to_double(&temp));
        LOG_INF("Humidity: %0.3f %%", sensor_value_to_double(&hum));

        k_sleep(K_MSEC(1000));
    }
}
