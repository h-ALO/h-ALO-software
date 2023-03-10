#include "egadc.h"

#include "MCP356X.h"

#include <zephyr/drivers/spi.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>


LOG_MODULE_REGISTER(adc_mcp356x, LOG_LEVEL_DBG);




int transceive(const struct spi_dt_spec *bus, uint8_t *tx, uint8_t *rx, uint8_t reg, uint8_t len, uint8_t cmd)
{
	struct spi_buf buf_tx[] = {{.buf = tx,.len = len+1}};
	struct spi_buf buf_rx[] = {{.buf = rx,.len = len+1}};
	struct spi_buf_set tx_buf = {.buffers = buf_tx, .count = 1};
	struct spi_buf_set rx_buf = {.buffers = buf_rx, .count = 1};
	tx[0] = MCP356X_COMMAND_BYTE(MCP356X_DEVICE_ADR, reg, cmd);
	int rv = spi_transceive_dt(bus, &tx_buf, &rx_buf);
	return rv;
}



int get(const struct spi_dt_spec *bus, uint8_t reg, uint32_t * value)
{
	uint8_t len = MCP356X_get_len(reg);
	uint8_t tx[5] = {0};
	uint8_t rx[5] = {0};
	int rv = transceive(bus, tx, rx, reg, len, MCP356X_CMD_INC_READ);
	*value = MCP356X_get_value(rx, len);
	//printk("MCP356X_get_value: %2i : %02X : %02X %02X %02X %02X : %5i\n", len, rx[0], rx[1], rx[2], rx[3], rx[4], v);
	return rv;
}

/*
In MUX mode, channel is defaulted to 0.
*/
int get_data_11(const struct spi_dt_spec *bus, int32_t * value, uint32_t * channel)
{
	uint8_t reg = MCP356X_REG_ADC_DATA;
	uint8_t len = 4;
	uint8_t tx[5] = {0};
	uint8_t rx[5] = {0};
	int rv = transceive(bus, tx, rx, reg, len, MCP356X_CMD_INC_READ);
	MCP356X_ADC_DATA_decode_11(rx, value, channel);
	return rv;
}



#define MCP356X_LOG_SET_VALUE
static int set(const struct spi_dt_spec *bus, uint8_t reg, uint32_t value)
{
	uint8_t len = MCP356X_get_len(reg);
	uint8_t tx[5] = {0};
	uint8_t rx[5] = {0};
	MCP356X_set_value(tx, len, value);
	int rv = transceive(bus, tx, rx, reg, len, MCP356X_CMD_INC_WRITE);
#ifdef MCP356X_LOG_SET_VALUE
	uint32_t value2;
	rv = get(bus, reg, &value2);
	LOG_INF("Register set: %s: %08x %08x", MCP356X_REG_tostring(reg), value, value2);
	if(reg == MCP356X_REG_IRQ)
	{
		LOG_INF("EN_STP:        %i", !!(value2 & MCP356X_IRQ_MASK_EN_STP));
		LOG_INF("EN_FASTCMD:    %i", !!(value2 & MCP356X_IRQ_MASK_EN_FASTCMD));
		LOG_INF("MODE0:         %i", !!(value2 & MCP356X_IRQ_MASK_MODE0));
		LOG_INF("MODE1:         %i", !!(value2 & MCP356X_IRQ_MASK_MODE1));
		LOG_INF("POR_STATUS:    %i", !!(value2 & MCP356X_IRQ_MASK_POR_STATUS));
		LOG_INF("CRCCFG_STATUS: %i", !!(value2 & MCP356X_IRQ_MASK_CRCCFG_STATUS));
		LOG_INF("DR_STATUS:     %i", !!(value2 & MCP356X_IRQ_MASK_DR_STATUS));
		LOG_INF("UNIMPLEMENTED: %i", !!(value2 & MCP356X_IRQ_MASK_UNIMPLEMENTED));
	}
#endif
return rv;
}




static void drdy_callback(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins)
{
	struct mcp356x_config *config = CONTAINER_OF(cb, struct mcp356x_config, drdy_cb);
	config->num_irq++;
	k_sem_give(&config->drdy_sem);
	//struct mcp356x_data11 data;
	//mcp356x_data11_get(&config->bus, &data);
}





static void mcp356x_acquisition_thread(struct mcp356x_config * config)
{
	LOG_INF("mcp356x_acquisition_thread started!");
	while (true)
	{
		int err = 0;
		//k_sem_take(&config->acq_sem, K_FOREVER);
		k_sem_take(&config->drdy_sem, K_SECONDS(12));
		int32_t value = 0;
		uint32_t channel = 0;
		err = get_data_11(&config->bus, &value, &channel);
		if (err)
		{
			printk("mcp356x_acquisition_thread error: %i\n", err);
			continue;
		}
		if (channel < MCP356X_CHANNEL_COUNT)
		{
			config->n[channel]++;
			config->mv[channel] = MCP356X_raw_to_millivolt(value, config->vref_mv, config->gain_reg);
			config->sum[channel] += config->mv[channel];
		}

		// After 1000 samples then calculate average:
		if(config->n[channel] > 1000)
		{
			config->avg[channel] = config->sum[channel] / config->n[channel];
			config->n[channel] = 0;
			config->sum[channel] = 0;
		}
		config->num_drdy++;
	}
}





int egadc_init(struct mcp356x_config * config)
{
	LOG_INF("Init ADC MCP356X");



	set(&config->bus, MCP356X_REG_CFG_0,
	MCP356X_CFG_0_VREF_SEL_0 |
	MCP356X_CFG_0_CLK_SEL_2 |
	MCP356X_CFG_0_CS_SEL_0 |
	MCP356X_CFG_0_MODE_CONV
	);
	set(&config->bus, MCP356X_REG_CFG_1,
	MCP356X_CFG_1_PRE_1 |
	MCP356X_CFG_1_OSR_32 |
	MCP356X_CFG_1_DITHER_DEF
	);
	set(&config->bus, MCP356X_REG_CFG_2,
	MCP356X_CFG_2_BOOST_X_1 |
	//MCP356X_CFG_2_GAIN_X_1 |
	config->gain_reg |
	MCP356X_CFG_2_AZ_MUX_DIS |
	//MCP356X_CFG_2_AZ_VREF_EN |
	//MCP356X_CFG_2_AZ_FREQ_HIGH |
	0
	);
	set(&config->bus, MCP356X_REG_CFG_3,
	MCP356X_CFG_3_CONV_MODE_CONT |
	//MCP356X_CFG_3_DATA_FORMAT_DEF |
	MCP356X_CFG_3_DATA_FORMAT_CH_ADC |
	MCP356X_CFG_3_CRC_FORMAT_16 |
	MCP356X_CFG_3_CRC_COM_DIS |
	MCP356X_CFG_3_CRC_OFF_CAL_EN |
	MCP356X_CFG_3_CRC_GAIN_CAL_EN
	);
	set(&config->bus, MCP356X_REG_MUX,
	//MCP356X_MUX_VIN_POS_CH0 | 
	//MCP356X_MUX_VIN_POS_CH1 | 
	//MCP356X_MUX_VIN_POS_CH2 | 
	//MCP356X_MUX_VIN_POS_CH3 | 
	//MCP356X_MUX_VIN_POS_TEMP |
	//MCP356X_MUX_VIN_POS_AVDD | 
	//MCP356X_MUX_VIN_POS_VREF_EXT_PLUS|
	//MCP356X_MUX_VIN_NEG_CH1 |
	//MCP356X_MUX_VIN_POS_CH0 | 
	MCP356X_MUX_VIN_POS_CH5 | 
	MCP356X_MUX_VIN_NEG_AGND |
	0);
	set(&config->bus, MCP356X_REG_SCAN, 0);
	/*
	set24_verbose(&config->bus, MCP356X_REG_SCAN, 
	MCP356X_SCAN_CH0|
	MCP356X_SCAN_CH1|
	MCP356X_SCAN_CH2|
	MCP356X_SCAN_CH3|
	MCP356X_SCAN_CH4|
	MCP356X_SCAN_CH5|
	MCP356X_SCAN_CH6|
	MCP356X_SCAN_CH7|
	MCP356X_SCAN_VREF|
	MCP356X_SCAN_TEMP|
	MCP356X_SCAN_AVDD|
	MCP356X_SCAN_OFFSET|
	0);
	*/
	//set24_verbose(bus, MCP356X_REG_SCAN, MCP356X_SCAN_CH0);
	//set24_verbose(bus, MCP356X_REG_SCAN, MCP356X_SCAN_CH3);
	
	set(&config->bus, MCP356X_REG_IRQ, MCP356X_IRQ_MODE_LOGIC_HIGH);
	set(&config->bus, MCP356X_REG_OFFSET_CAL, 0);
	set(&config->bus, MCP356X_REG_GAIN_CAL, 0x00800000);
	//set24_verbose(MCP356X_RSV_REG_W_A, 0x00900F00);
	set(&config->bus, MCP356X_RSV_REG_W_A, 0x00900000);



	LOG_INF("Configuring port %s %i",config->irq.port->name, config->irq.pin);
	int err;
	err = gpio_pin_configure_dt(&config->irq, GPIO_INPUT);
	if (err) {return err;}
	err = gpio_pin_interrupt_configure_dt(&config->irq, GPIO_INT_EDGE_TO_ACTIVE);
	if (err) {return err;}
	gpio_init_callback(&config->drdy_cb, drdy_callback, BIT(config->irq.pin));
	err = gpio_add_callback(config->irq.port, &config->drdy_cb);
	if (err) {return err;}

	k_sem_init(&config->acq_sem, 0, 1);
	k_sem_init(&config->drdy_sem, 0, 1);

	k_thread_create(&config->thread, config->stack,
			ADC_MCP356X_ACQUISITION_THREAD_STACK_SIZE,
			(k_thread_entry_t)mcp356x_acquisition_thread,
			(void *)config, NULL, NULL,
			K_PRIO_COOP(ADC_MCP356X_ACQUISITION_THREAD_PRIO),
			//0,
			0, K_NO_WAIT);
	/* Add instance number to thread name? */
	k_thread_name_set(&config->thread, "mcp356x");

	return 0;
}






