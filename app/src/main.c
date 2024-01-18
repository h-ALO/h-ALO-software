/*
west build -b nucleo_wb55rg

Timer for nucleo_wb55rg:
https://github.com/zephyrproject-rtos/zephyr/discussions/47033
https://github.com/zephyrproject-rtos/zephyr/blob/main/samples/drivers/counter/alarm/src/main.c

*/

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/sys/util.h>
#include <stdint.h>
#include <stdio.h>


#include "app_version.h"
#include "egadc.h"
#include "bt.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);
//struct spi_dt_spec bus = SPI_DT_SPEC_GET(DT_NODELABEL(mcp3204), SPI_WORD_SET(8) | SPI_MODE_GET(0), 1);


// The ADC9 uses 2000mV voltage ref chip MCP1501
#define ADC9_VREF_MICRO_VOLT 2048*1000
#define TIABT_VREF_MICRO_VOLT 3000*1000




//#define TIMER DT_NODELABEL(counters2)

#if defined(CONFIG_BOARD_ATSAMD20_XPRO)
#define TIMER DT_NODELABEL(tc4)
#elif defined(CONFIG_SOC_FAMILY_SAM)
#define TIMER DT_NODELABEL(tc0)
#elif defined(CONFIG_COUNTER_MICROCHIP_MCP7940N)
#define TIMER DT_NODELABEL(extrtc0)
#elif defined(CONFIG_COUNTER_NRF_RTC)
#define TIMER DT_NODELABEL(rtc0)
#elif defined(CONFIG_COUNTER_TIMER_STM32)
#define TIMER DT_INST(0, st_stm32_counter)
#elif defined(CONFIG_COUNTER_RTC_STM32)
#define TIMER DT_INST(0, st_stm32_rtc)
#elif defined(CONFIG_COUNTER_SMARTBOND_TIMER)
#define TIMER DT_NODELABEL(timer3)
#elif defined(CONFIG_COUNTER_NATIVE_POSIX)
#define TIMER DT_NODELABEL(counter0)
#elif defined(CONFIG_COUNTER_XLNX_AXI_TIMER)
#define TIMER DT_INST(0, xlnx_xps_timer_1_00_a)
#elif defined(CONFIG_COUNTER_TMR_ESP32)
#define TIMER DT_NODELABEL(timer0)
#elif defined(CONFIG_COUNTER_MCUX_CTIMER)
#define TIMER DT_NODELABEL(ctimer0)
#elif defined(CONFIG_COUNTER_NXP_S32_SYS_TIMER)
#define TIMER DT_NODELABEL(stm0)
#elif defined(CONFIG_COUNTER_TIMER_GD32)
#define TIMER DT_NODELABEL(timer0)
#elif defined(CONFIG_COUNTER_GECKO_RTCC)
#define TIMER DT_NODELABEL(rtcc0)
#elif defined(CONFIG_COUNTER_GECKO_STIMER)
#define TIMER DT_NODELABEL(stimer0)
#elif defined(CONFIG_COUNTER_INFINEON_CAT1)
#define TIMER DT_NODELABEL(counter0_0)
#elif defined(CONFIG_COUNTER_AMBIQ)
#define TIMER DT_NODELABEL(counter0)
#elif defined(CONFIG_COUNTER_SNPS_DW)
#define TIMER DT_NODELABEL(timer0)
#endif




#if DT_NODE_HAS_STATUS(TIMER, okay)
const struct device *const counter_dev = DEVICE_DT_GET(TIMER);
#else
#error "Node is disabled"
#endif


struct counter_alarm_cfg alarm_cfg;


void app_print_voltage(struct mcp356x_config * c)
{
	// In single channel mode, any channel will be stored in channel 0:
	int index = MCP356X_CH_CH0;
	int32_t v_iir = MCP356X_raw_to_volt(c->raw_iir[index], TIABT_VREF_MICRO_VOLT, c->gain_reg);
	int32_t v_min = MCP356X_raw_to_volt(c->raw_min[index], TIABT_VREF_MICRO_VOLT, c->gain_reg);
	int32_t v_max = MCP356X_raw_to_volt(c->raw_max[index], TIABT_VREF_MICRO_VOLT, c->gain_reg);
	int32_t v_pp = v_max - v_min;
	printk("IRQ:%-3i DRDY:%-3i avg:%-8i min:%-8i max:%-8i pp:%-8i\n", c->num_irq, c->num_drdy, v_iir, v_min, v_max, v_pp);
	printk("IRQ:%-3i DRDY:%-3i avg:%-8i min:%-8i max:%-8i pp:%-8i\n", c->num_irq, c->num_drdy, v_iir, v_min, v_max, v_pp);
}

void app_print_voltage_ref(struct mcp356x_config * c)
{
	LOG_INF("Checking voltage reference");
	egadc_adc_value_reset(c);
	egadc_set_mux(c, MCP356X_MUX_VIN_NEG_AGND | MCP356X_MUX_VIN_POS_VREF_EXT_PLUS);
	int n = 10;
	while (n--)
	{
		app_print_voltage(c);
		egadc_adc_value_reset(c);
		k_sleep(K_MSEC(500));
	}
}

void app_print_temperature(struct mcp356x_config * c)
{
	egadc_adc_value_reset(c);
	LOG_INF("Checking temperature MCP356X_MUX_VIN_POS_TEMP");
	egadc_set_ch(c, MCP356X_CH_TEMP);
	int n = 10;
	while (n--)
	{
		int raw = c->raw_iir[MCP356X_CH_CH0];
		// TODO: I can't figure out why datasheet temperature transfer function does not work.
		//double celsius  = MCP356X_ADCDATA_to_temperature_o1(raw) * ((double)TIABT_VREF/(double)3300);
		//double celcius  = MCP356X_ADCDATA_to_temperature_o3(raw);

		// If we convert raw ADCDATA to millivolt then use datasheet celsius to mv function (we get correct temperature?)
		// https://www.eevblog.com/forum/microcontrollers/problems-with-internal-temperature-sensor-mcp3561/
		int32_t v = MCP356X_raw_to_volt(raw, TIABT_VREF_MICRO_VOLT, c->gain_reg);
		double celsius = ((double)v -79.32) / (0.2964*10000);
		printf("%08i %08i %20.10f C\n", raw, v, celsius);
		//printf("%04i %f C\n", voltage, celcius * (c->vref_mv / 3.3));
		k_sleep(K_MSEC(500));
	}
}









enum app_state
{
	APP_START,
	APP_WAITING,
	APP_INIT_ADC,
	APP_PRINT_ADC
};



struct mcp356x_config c = 
{
	.bus = SPI_DT_SPEC_GET(DT_NODELABEL(examplesensor0), SPI_WORD_SET(8) | SPI_MODE_GET(0), 1),
	.irq = GPIO_DT_SPEC_GET(DT_NODELABEL(examplesensor0), irq_gpios),
	.is_scan = false,
	//.gain_reg = MCP356X_CFG_2_GAIN_X_033,
	.gain_reg = MCP356X_CFG_2_GAIN_X_1,
};

enum my_leds
{
	MY_LEDS_OTHER,
	MY_LEDS_BEATS,
	MY_LEDS_WAITING,
	MY_LEDS_COUNT
};

#define LEDS_COUNT 3
static const struct gpio_dt_spec leds[LEDS_COUNT] = 
{
	GPIO_DT_SPEC_GET(DT_NODELABEL(blue_led_1), gpios), //Blue
	GPIO_DT_SPEC_GET(DT_NODELABEL(green_led_2), gpios), //Green
	GPIO_DT_SPEC_GET(DT_NODELABEL(green_led_3), gpios), //Red
};

#define DELAY 2000000
#define ALARM_CHANNEL_ID 0




static void test_counter_interrupt_fn(const struct device *counter_dev,
				      uint8_t chan_id, uint32_t ticks,
				      void *user_data)
{
	struct counter_alarm_cfg *config = user_data;
	uint32_t now_ticks;
	uint64_t now_usec;
	int now_sec;
	int err;

	err = counter_get_value(counter_dev, &now_ticks);
	if (err) {
		printk("Failed to read counter value (err %d)", err);
		return;
	}

	now_usec = counter_ticks_to_us(counter_dev, now_ticks);
	now_sec = (int)(now_usec / USEC_PER_SEC);

	printk("!!! Alarm !!!\n");
	printk("Now: %u\n", now_sec);

	/* Set a new alarm with a double length duration */
	config->ticks = config->ticks * 2U;

	printk("Set alarm in %u sec (%u ticks)\n",
	       (uint32_t)(counter_ticks_to_us(counter_dev,
					   config->ticks) / USEC_PER_SEC),
	       config->ticks);

	err = counter_set_channel_alarm(counter_dev, ALARM_CHANNEL_ID,
					user_data);
	if (err != 0) {
		printk("Alarm could not be set\n");
	}
}
















int main(void)
{
	LOG_INF("Zephyr Example Application %s", "" DT_NODE_PATH(DT_NODELABEL(examplesensor0)));

	
	if (!device_is_ready(counter_dev))
	{
		LOG_ERR("Timer device not ready.\n");
		return 0;
	}

	if (!spi_is_ready_dt(&c.bus))
	{
		LOG_ERR("SPI bus is not ready %i", 0);
		return 0;
	}

	/*
	counter_start(counter_dev);
	alarm_cfg.flags = 0;
	alarm_cfg.ticks = counter_us_to_ticks(counter_dev, DELAY);
	alarm_cfg.callback = test_counter_interrupt_fn;
	alarm_cfg.user_data = &alarm_cfg;
	printk("Set alarm in %u sec (%u ticks)\n",
	       (uint32_t)(counter_ticks_to_us(counter_dev,
					   alarm_cfg.ticks) / USEC_PER_SEC),
	       alarm_cfg.ticks);
		   
	{
		int err = counter_set_channel_alarm(counter_dev, ALARM_CHANNEL_ID, &alarm_cfg);
		if (-EINVAL == err) {
			printk("Alarm settings invalid\n");
		} else if (-ENOTSUP == err) {
			printk("Alarm setting request not supported\n");
		} else if (err != 0) {
			printk("Error\n");
		}
	}
	*/


	//mybt_init();
	//while (1){k_sleep(K_MSEC(5000));}




	for(int i = 0; i < LEDS_COUNT; ++i)
	{
		int ret;
		if (!gpio_is_ready_dt(leds+i)) {return 0;}
		ret = gpio_pin_configure_dt(leds+i, GPIO_OUTPUT_ACTIVE);
		if (ret < 0) {return 0;}
		k_sleep(K_MSEC(500));
		ret = gpio_pin_set_dt(leds+i,1);
		if (ret < 0) {return 0;}
	}
	k_sleep(K_MSEC(500));
	for(int i = 0; i < LEDS_COUNT; ++i)
	{
		int ret = gpio_pin_set_dt(leds+i,0);
		if (ret < 0) {return 0;}
	}



	int appstate = APP_START;

	while (1)
	{
		{
			int ret = gpio_pin_toggle_dt(leds+MY_LEDS_BEATS);
			if (ret < 0) {return 0;}
		}

		if(c.status & EGADC_TIMEOUT_BIT)
		{
			appstate = APP_INIT_ADC;
			c.status &= ~EGADC_TIMEOUT_BIT;
		}


		switch (appstate)
		{
		case APP_WAITING:
			if(c.num_drdy > 0)
			{
				//app_print_voltage_ref(&c);
				//app_print_temperature(&c);
				egadc_set_ch(&c, MCP356X_CH_CH3);
				appstate = APP_PRINT_ADC;
			}
			else
			{
				LOG_INF("APP_WAITING No respond from ADC");
				gpio_pin_set_dt(leds + MY_LEDS_WAITING, 1);
				k_sleep(K_MSEC(500));
				gpio_pin_set_dt(leds + MY_LEDS_WAITING, 0);
				k_sleep(K_MSEC(500));
			}
			break;

		case APP_START:
			LOG_INF("APP_START");
			egadc_setup_board(&c);
			appstate = APP_INIT_ADC;
			break;

		case APP_INIT_ADC:
			LOG_INF("APP_INIT_ADC");
			egadc_setup_adc(&c);
			appstate = APP_WAITING;
			break;

		case APP_PRINT_ADC:{
			k_sleep(K_MSEC(1000));
			app_print_voltage(&c);
			egadc_adc_value_reset(&c);
			break;}

		default:
			break;
		}




		//printk("%08X\n", c.lastdata);
		
		
		//printk("%8i %8i\n", c.num_irq, c.num_drdy);
		//printk("    " MCP356X_PRINTF_HEADER "\n");
		//printk("avg " MCP356X_PRINTF_PLUS "\n", MCP356X_ARGS(c.avg));
		//printk("min " MCP356X_PRINTF_PLUS "\n", MCP356X_ARGS(c.val_min));
		//printk("max " MCP356X_PRINTF_PLUS "\n", MCP356X_ARGS(c.val_max));
		//printk("n   " MCP356X_PRINTF_PLUS "\n", MCP356X_ARGS(c.n));
		
		//egadc_log_REG_IRQ(&c.bus, MCP356X_REG_IRQ);

		
		
		//mybt_progress();
		//bt_bas_set_battery_level(i++);

		//k_sem_give(&c.acq_sem);
	}
}


