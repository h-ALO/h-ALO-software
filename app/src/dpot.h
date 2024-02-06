#pragma once
#include <zephyr/drivers/i2c.h>

// MCP45HVX1
struct mcp45hvx1_config {
	const struct i2c_dt_spec bus;
};


int dpot_set(struct mcp45hvx1_config * config, int value);

int dpot_setup(struct mcp45hvx1_config * config);