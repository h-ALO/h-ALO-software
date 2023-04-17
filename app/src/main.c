/*
west build -b nucleo_wb55rg

*** Booting Zephyr OS build zephyr-v3.3.0-1017-g1545f9ba7f33 ***
[00:00:00.005,000] <ESC>[0m<inf> main: Zephyr Example Application /soc/spi@40013000/examplesensor@0<ESC>[0m
*** Booting Zephyr OS build zephyr-v3.3.0-1928-g0e6c306dcec9 ***
[00:00:00.005,000] <ESC>[0m<inf> main: Zephyr Example Application /soc/spi@40013000/examplesensor@0<ESC>[0m
[00:00:00.014,000] <ESC>[0m<inf> adc_mcp356x: Setting registers in MCP356X<ESC>[0m
[00:00:00.021,000] <ESC>[0m<inf> adc_mcp356x: Register set: MCP356X_REG_LOCK        : 000000a5 00000000<ESC>[0m
[00:00:00.030,000] <ESC>[0m<inf> adc_mcp356x: Register set: MCP356X_REG_CFG_0       : 00000023 00000000<ESC>[0m




*** Booting Zephyr OS build zephyr-v3.3.0-1928-g0e6c306dcec9 ***
*** Booting Zephyr OS build zephyr-v3.3.0-1928-g0e6c306dcec9 ***
*** Booting Zephyr OS build zephyr-v3.3.0-2655-g9e105d203766 ***



*/

#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/spi.h>
#include <stdint.h>

#include "app_version.h"
#include "egadc.h"
#include "bt.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);
//struct spi_dt_spec bus = SPI_DT_SPEC_GET(DT_NODELABEL(mcp3204), SPI_WORD_SET(8) | SPI_MODE_GET(0), 1);


// The ADC9 uses 2000mV voltage ref chip MCP1501
#define ADC9_VREF 2048
#define TIABT_VREF 3000





void app_print_voltage_ref(struct mcp356x_config * c)
{
	LOG_INF("Checking voltage reference:");
	egadc_adc_value_reset(c);
	egadc_set_mux(c, MCP356X_MUX_VIN_NEG_AGND | MCP356X_MUX_VIN_POS_VREF_EXT_PLUS);
	int n = 10;
	while (n--)
	{
		printk("IRQ:%-4i DRDY:%-4i avg:%-8i min:%-8i max:%-8i\n", c->num_irq, c->num_drdy, c->mv_iir[MCP356X_CH_CH0], c->mv_min[MCP356X_CH_CH0], c->mv_max[MCP356X_CH_CH0]);
		egadc_adc_value_reset(c);
		k_sleep(K_MSEC(1000));
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
	.gain_reg = MCP356X_CFG_2_GAIN_X_033,
	.vref_mv = TIABT_VREF
};




void main(void)
{
	LOG_INF("Zephyr Example Application %s", "" DT_NODE_PATH(DT_NODELABEL(examplesensor0)));

	if (!spi_is_ready_dt(&c.bus))
	{
		LOG_ERR("SPI bus is not ready %i", 0);
		return;
	}

	//mybt_init();
	//while (1){k_sleep(K_MSEC(5000));}


	while(0)
	{
		LOG_INF("1");
		k_sleep(K_MSEC(1000));
	}


	int appstate = APP_START;
	while (1)
	{
		if(c.status & EGADC_TIMEOUT_BIT)
		{
			appstate = APP_INIT_ADC;
		}


		switch (appstate)
		{
		case APP_WAITING:
			LOG_INF("APP_WAITING");
			if(c.status & EGADC_THREAD_STARTED_BIT)
			{
				app_print_voltage_ref(&c);
				egadc_set_mux(&c, MCP356X_MUX_VIN_NEG_AGND | MCP356X_MUX_VIN_POS_CH5);
				appstate = APP_PRINT_ADC;
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

		case APP_PRINT_ADC:
			printk("IRQ:%-4i DRDY:%-4i avg:%-8i min:%-8i max:%-8i\n", c.num_irq, c.num_drdy, c.mv_iir[MCP356X_CH_CH0], c.mv_min[MCP356X_CH_CH0], c.mv_max[MCP356X_CH_CH0]);
			egadc_adc_value_reset(&c);
			break;

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
		k_sleep(K_MSEC(1000));
	}
}

