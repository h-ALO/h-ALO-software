#pragma once
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include "MCP356X.h"

#define ADC_MCP356X_ACQUISITION_THREAD_STACK_SIZE 1024
#define ADC_MCP356X_ACQUISITION_THREAD_PRIO 20
#define ADC_MCP356X_MAX_MSGS 8

struct mcp356x_sample
{
	int32_t avg;
	int32_t min;
	int32_t max;
};


struct mcp356x_config
{
	const struct spi_dt_spec bus;
	struct gpio_dt_spec irq; /* GPIO  */
	struct k_sem acq_sem;		/* Signal acq thread for next sample */
	struct k_sem drdy_sem;		/* Signal data ready IRQ */
	struct gpio_callback drdy_cb;	/* For data ready IRQ */
	struct k_thread thread;		/* Acquisition thread */
	int num_irq;
	int num_drdy;


	char __aligned(4) msgq_buffer[ADC_MCP356X_MAX_MSGS * sizeof(struct mcp356x_sample)];
	struct k_msgq msgq;


	int n[MCP356X_CHANNEL_COUNT];
	int mv_avg[MCP356X_CHANNEL_COUNT];
	int mv_min[MCP356X_CHANNEL_COUNT];
	int mv_max[MCP356X_CHANNEL_COUNT];

	int is_scan;
	uint8_t gain_reg; /* Gain register value */
	uint16_t vref_mv; /* Voltage reference millivolt value */
	K_KERNEL_STACK_MEMBER(stack, ADC_MCP356X_ACQUISITION_THREAD_STACK_SIZE);
};


int egadc_init(struct mcp356x_config * config);
int egadc_log_REG_IRQ(const struct spi_dt_spec *bus, uint8_t reg);