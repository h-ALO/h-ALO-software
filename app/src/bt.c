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
	LOG_INF("read_u16 %i", value);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &value,sizeof(value));
}

static ssize_t write_u16(struct bt_conn *conn, const struct bt_gatt_attr *attr,void *buf, uint16_t len, uint16_t offset)
{
	const uint16_t *u16 = attr->user_data;
	LOG_INF("write_u16 %i", (int)(*u16));
	return len;
}

//static struct bt_uuid_128 service1_uuid = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0000fe40, 0xcc7a, 0x482a, 0x984a, 0x7f2ed5b3e58f));

// #define CUSTOM_UUID(val) ("2A5A20B9-" val "-4B9C-9C69-4975713E0FF2")

typedef enum varid_t {
	VARID_SRV0,
	VARID_SRV0_CH0,
	VARID_SRV0_CH1,
	VARID_SRV0_CH2,
	VARID_SRV0_CH3,
	VARID_SRV0_CH4,
	VARID_SRV0_CH5,
	VARID_SRV0_CH6,
	VARID_SRV0_CH7,
};

#define MY_UUID(x) BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x2A5A20B9, (x), 0x4B9C, 0x9C69, 0x4975713E0FF2))
static const struct bt_uuid_128 uuid_srv0 = MY_UUID(VARID_SRV0);
static const struct bt_uuid_128 uuid_srv0_ch0 = MY_UUID(VARID_SRV0_CH0);
static const struct bt_uuid_128 uuid_srv0_ch1 = MY_UUID(VARID_SRV0_CH1);
/*
static const struct bt_uuid_128 st_service_uuid = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0000fe40, 0xcc7a, 0x482a, 0x984a, 0x7f2ed5b3e58f));
static const struct bt_uuid_128 led_char_uuid = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0000fe41, 0x8e22, 0x4541, 0x9d4c, 0x21edae82ed19));
static const struct bt_uuid_128 but_notif_uuid = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0000fe42, 0x8e22, 0x4541, 0x9d4c, 0x21edae82ed19));
*/

struct gatt_values
{
	int32_t voltage[2];
	const struct bt_gatt_attr *attr[10];
};

static struct gatt_values values;


static void ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value);


static ssize_t write_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,const void *buf, uint16_t len, uint16_t offset,uint8_t flags)
{
	uint16_t *value = attr->user_data;
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


//2.4.1 GATT Format Types
#define FORMAT_FLOAT32 0x14
#define FORMAT_SINT32 0x10

// 3.5 Units
#define UNIT_ACCELERATION 0x2713
#define UNIT_VOLTAGE 0x2728
#define UNIT_SECOND 0x2703

static struct bt_gatt_cpf cha_format_value =
{
	FORMAT_SINT32, // 2.4.1 GATT Format Types
	0x00, // exponent
	UNIT_VOLTAGE, // 3.5.1 Units by Name
	0x01, // 2.4.2 GATT Characteristic Presentation Format Name Space
	0x010F // 2.4.2.1 Bluetooth SIG GATT Characteristic Presentation Format Description
};



BT_GATT_SERVICE_DEFINE(service1,
	BT_GATT_PRIMARY_SERVICE(&uuid_srv0),
	//BT_GATT_CHARACTERISTIC(&led_char_uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ, read_u16, NULL, &values.voltage[0]),
	BT_GATT_CHARACTERISTIC(&uuid_srv0_ch0, BT_GATT_CHRC_NOTIFY, 0, NULL, NULL, &values.voltage[0]),
	BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CUD("CH0", BT_GATT_PERM_READ),
	BT_GATT_CPF(&cha_format_value),

	BT_GATT_CHARACTERISTIC(&uuid_srv0_ch1, BT_GATT_CHRC_NOTIFY, 0, NULL, NULL, &values.voltage[1]),
	BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CUD("CH1", BT_GATT_PERM_READ),
	BT_GATT_CPF(&cha_format_value),
);

int simulate_temp = 0;
static void ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	simulate_temp = value == BT_GATT_CCC_NOTIFY;
	void * v = service1.attrs[1].user_data;
	LOG_INF("ccc_cfg_changed %p %p", v, &values.voltage[0]);
	if (v == &values.voltage[0]){
		printk("ccc_cfg_changed 0");
	}
	else if (v == &values.voltage[0]){
		printk("ccc_cfg_changed 1");
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
	if(simulate_temp) {
		printk("bt_gatt_notify\n");
		bt_gatt_notify(NULL, &service1.attrs[1], &values.voltage[0], sizeof(values.voltage[0]));
	}
	
	//bt_gatt_notify(NULL, attr, &value, sizeof(value));
}