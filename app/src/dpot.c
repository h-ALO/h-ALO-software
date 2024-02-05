#include "dpot.h"
#include "MCP45HVX1.h"


void write(struct mcp45hvx1_config * config, int addr, int cmd, int value)
{
	printk("write : I2C(%02X) %s %02X\n", addr, MCP45HVX1_REG_tostring(cmd & 0xF0), value);
	uint8_t data_to_write[2]; // Data to write to the register
    data_to_write[0] = cmd;
    data_to_write[1] = value;
	struct i2c_msg msgs[1];
	msgs[0].buf = data_to_write;
	msgs[0].len = sizeof(data_to_write);
	msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

	// Start the I2C communication
	int ret = i2c_transfer(config->bus.bus, &msgs[0], 1, addr);
	if (ret) {
	printk("Error writing to register: %d\n", ret);
	return;
	}
}


void test(struct mcp45hvx1_config * config, int addr, int cmd)
{
	uint8_t data_to_write = cmd; // Data to write to the register
	uint8_t read_data[2]; // Data read from the register
	struct i2c_msg msgs[2];
	msgs[0].buf = &data_to_write;
	msgs[0].len = sizeof(data_to_write);
	msgs[0].flags = I2C_MSG_WRITE;
	msgs[1].buf = read_data;
	msgs[1].len = sizeof(read_data);
	msgs[1].flags = I2C_MSG_READ | I2C_MSG_STOP;

	// Start the I2C communication
	int ret = i2c_transfer(config->bus.bus, &msgs[0], 1, addr);
	if (ret) {
	printk("Error writing to register: %d\n", ret);
	return;
	}

	// Delay (if needed) to ensure the write operation has taken effect

	// Start the I2C communication for read
	ret = i2c_transfer(config->bus.bus, &msgs[1], 1, addr);
	if (ret) {
	printk("Error reading from register: %d\n", ret);
	return;
	}

	
	printk("read : I2C(%02X) %s : %02X %02X\n", addr, MCP45HVX1_REG_tostring(cmd & 0xF0), read_data[0], read_data[1]);

	// Print the read data
	//printk("Read data from register: 0x%02X 0x%02X\n", read_data[0], read_data[1]);
}

int dpot_setup(struct mcp45hvx1_config * config)
{
    write(config, 0x3C, MCP45HVX1_MEM_TCON | MCP45HVX1_COM_WRITE, MCP45HVX1_TCON_R0HW | MCP45HVX1_TCON_R0A | MCP45HVX1_TCON_R0W);
    //write(config, 0x3C, MEM_TCON | COM_WRITE, 0);
    write(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_WRITE, 0);
    test(config, 0x3C, MCP45HVX1_MEM_TCON | MCP45HVX1_COM_READ);
    test(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_READ);

    //test(config, 0x3C, MEM_WIPER | COM_READ);
    //test(config, 0x3D);
    //test(config, 0x3E);
    //test(config, 0x3F);
}