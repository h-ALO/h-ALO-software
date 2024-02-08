#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#include <zephyr/types.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/services/bas.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(adc_svc, CONFIG_APP_LOG_LEVEL);

static uint8_t ct[10];
static uint8_t ct_update;

static ssize_t read_u16(struct bt_conn *conn, const struct bt_gatt_attr *attr,void *buf, uint16_t len, uint16_t offset)
{
	const uint16_t *u16 = attr->user_data;
	uint16_t value = sys_cpu_to_le16(*u16);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &value,sizeof(value));
}

static ssize_t write_u16(struct bt_conn *conn, const struct bt_gatt_attr *attr,void *buf, uint16_t len, uint16_t offset)
{
	const uint16_t *u16 = attr->user_data;
	LOG_INF("write_u16 %i", (int)(*u16));
	return len;
}

static struct bt_uuid_128 service1_uuid = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0000fe40, 0xcc7a, 0x482a, 0x984a, 0x7f2ed5b3e58f));



struct gatt_values
{
	uint16_t voltage[2];
	const struct bt_gatt_attr *attr[10];
};

static struct gatt_values values;


static void ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value);


static ssize_t write_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,const void *buf, uint16_t len, uint16_t offset,uint8_t flags)
{
	uint8_t *value = attr->user_data;
	if (offset + len > sizeof(ct)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}
	memcpy(value + offset, buf, len);
	ct_update = 1U;
	return len;
}

static ssize_t read_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	const char *value = attr->user_data;
	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,sizeof(ct));
}



BT_GATT_SERVICE_DEFINE(service1,
	BT_GATT_PRIMARY_SERVICE(&service1_uuid),
	BT_GATT_CHARACTERISTIC(BT_UUID_GATT_V, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_u16, write_u16, &values.voltage[0]),
	BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(BT_UUID_GATT_V, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_u16, write_u16, &values.voltage[1]),
	BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(BT_UUID_CTS_CURRENT_TIME, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,read_ct, write_ct, ct),
	BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

static void ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	uint16_t * v = service1.attrs[0].user_data;
	LOG_INF("ccc_cfg_changed %i", (int)(*v));
	if (attr->user_data == &values.voltage[0])
	{
		LOG_INF("ccc_cfg_changed 0");
	}
	else if (attr->user_data == &values.voltage[0])
	{
		LOG_INF("ccc_cfg_changed 1");
	}
}


static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("Connection failed (err 0x%02x)\n", err);
	} else {
		printk("Connected\n");
	}
	
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason 0x%02x)\n", reason);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};



static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR))
};



static void bt_ready(int err)
{
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)", err);
		return;
	}
	LOG_INF("Bluetooth initialized. Connecting and disconnecting and connecting fails.");
	/* Start advertising */
	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		LOG_ERR("Advertising failed to start (err %d)", err);
		return;
	}

	LOG_INF("Configuration mode: waiting connections...");
}


static void bas_notify(void)
{
	uint8_t battery_level = bt_bas_get_battery_level();
	battery_level--;
	if (!battery_level) {
		battery_level = 100U;
	}
	bt_bas_set_battery_level(battery_level);
}




void mybt_init(void)
{
	int err;
	printk("bt_enable\n", err);
	err = bt_enable(bt_ready);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}
}



void mybt_progress(void)
{
    bas_notify();
	values.voltage[0] += 1;
	values.voltage[1] += 1;

	
	//bt_gatt_notify(NULL, attr, &value, sizeof(value));
}