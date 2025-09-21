#include "temperature_sensor_service.h"
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(temperature_sensor_service, LOG_LEVEL_INF);

// Variavel global para a caracteristica de leitura "Temp Data".
static int16_t temp_data_var;

// Flag global para notificacoes de "Temp Data".
bool temp_data_notify_enabled;

static void temp_data_ccc_change(const struct bt_gatt_attr *attr, uint16_t value)
{
    temp_data_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    LOG_INF("Temp Data notifications %s", temp_data_notify_enabled ? "enabled" : "disabled");
}

// Variavel global para a caracteristica de leitura "Sampling Interval".
extern uint8_t sampling_interval_var;

static ssize_t on_temp_data_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
    void *buf, uint16_t len, uint16_t offset) {
    // TODO: Implementar callback de leitura para "Temp Data"
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &temp_data_var, sizeof(temp_data_var));
}

static ssize_t on_sampling_interval_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
    void *buf, uint16_t len, uint16_t offset) {
    // TODO: Implementar callback de leitura para "Sampling Interval"
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &sampling_interval_var, sizeof(sampling_interval_var));
}
static ssize_t on_sampling_interval_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
    const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {
    
    if (len == sizeof(uint8_t)) {
            uint8_t new_interval = *((uint8_t *)buf);
            sampling_interval_var = new_interval;
            update_sensor_timer_interval(new_interval); // Chama a função do main
            return len;
        }
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
}

BT_GATT_SERVICE_DEFINE(temperature_sensor_service_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_128(TEMPERATURE_SENSOR_SERVICE_UUID)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_128(TEMP_DATA_CHAR_UUID),
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           on_temp_data_read, NULL, &temp_data_var),
    BT_GATT_CCC(temp_data_ccc_change, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_128(SAMPLING_INTERVAL_CHAR_UUID),
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           on_sampling_interval_read, on_sampling_interval_write, &sampling_interval_var)
);

/* Envia notificacao para a caracteristica temp_data */
int send_temp_data_notification(int16_t temp_data)
{
    temp_data_var = temp_data;
    if (!temp_data_notify_enabled) {
        return -EACCES;
    }
    /* O atributo de valor esta assumido no indice 2 na estrutura do servico. */
    return bt_gatt_notify(NULL, &temperature_sensor_service_svc.attrs[2],
                            &temp_data_var, sizeof(temp_data_var));
}


int temperature_sensor_service_init(void) {
    LOG_INF("Temperature Sensor Service service initialized");
    return 0;
}