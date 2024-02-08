#include "dpot.h"
#include "MCP45HVX1.h"



void transfer(struct mcp45hvx1_config * config, int addr, int cmd, int value)
{
	uint8_t tx[2] = {cmd, value};
	uint8_t rx[2] = {0};
	struct i2c_msg msgs[2] = {
		{.buf = tx},
		{.buf = rx},
	};
	int rc = 0;


	switch (cmd & 0x0F) {
	case MCP45HVX1_COM_READ:
		msgs[0].flags = I2C_MSG_WRITE;
		msgs[1].flags = I2C_MSG_READ | I2C_MSG_STOP;
		msgs[0].len = 1;
		msgs[1].len = 2;
		rc = i2c_transfer(config->bus.bus, msgs + 0, 1, addr);
		if (rc) {
			printk("Error i2c_transfer: %d\n", rc);
			return;
		}
		rc = i2c_transfer(config->bus.bus, msgs + 1, 1, addr);
		if (rc) {
			printk("Error i2c_transfer: %d\n", rc);
			return;
		}
		break;
	
	case MCP45HVX1_COM_WRITE:
		msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;
		msgs[0].len = 2;
		rc = i2c_transfer(config->bus.bus, msgs + 0, 1, addr);
		if (rc) {
			printk("Error i2c_transfer: %d\n", rc);
			return;
		}
		break;
	case MCP45HVX1_COM_WIPERINC:
	case MCP45HVX1_COM_WIPERDEC:
		msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;
		msgs[0].len = 1;
		rc = i2c_transfer(config->bus.bus, msgs + 0, 1, addr);
		if (rc) {
			printk("Error i2c_transfer: %d\n", rc);
			return;
		}
		break;
	default:
		break;
	}

	switch (cmd & 0x0F) {
	case MCP45HVX1_COM_WIPERINC:
		printk("MCP45HVX1 WIPERINC: %02X/%s\n", addr, MCP45HVX1_REG_tostring(cmd & 0xF0));
		break;
	case MCP45HVX1_COM_WIPERDEC:
		printk("MCP45HVX1 WIPERDEC: %02X/%s\n", addr, MCP45HVX1_REG_tostring(cmd & 0xF0));
		break;
	case MCP45HVX1_COM_READ:
		printk("MCP45HVX1 READ: %02X/%s: %02X %02X\n", addr, MCP45HVX1_REG_tostring(cmd & 0xF0), rx[0], rx[1]);
		break;
	case MCP45HVX1_COM_WRITE:
		printk("MCP45HVX1 WRITE: %02X/%s: %02X\n", addr, MCP45HVX1_REG_tostring(cmd & 0xF0), value);
		break;
	}

}



int dpot_set(struct mcp45hvx1_config * config, int value)
{
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_WRITE, value);
	//transfer(config, 0x3C, MCP45HVX1_MEM_TCON | MCP45HVX1_COM_READ, 0);
	return 0;
}



int dpot_setup(struct mcp45hvx1_config * config)
{
	transfer(config, 0x3C, MCP45HVX1_MEM_TCON | MCP45HVX1_COM_WRITE, MCP45HVX1_TCON_R0HW | MCP45HVX1_TCON_R0A | MCP45HVX1_TCON_R0W);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_WRITE, 255);
	transfer(config, 0x3C, MCP45HVX1_MEM_TCON | MCP45HVX1_COM_READ, 0);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_READ, 0);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_WIPERDEC, 0);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_READ, 0);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_WIPERDEC, 0);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_READ, 0);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_WIPERDEC, 0);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_READ, 0);
	
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_WRITE, 4);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_WIPERINC, 0);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_READ, 0);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_WIPERINC, 0);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_READ, 0);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_WIPERINC, 0);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_READ, 0);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_WIPERINC, 0);
	transfer(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_READ, 0);


    //write(config, 0x3C, MCP45HVX1_MEM_TCON | MCP45HVX1_COM_WRITE, MCP45HVX1_TCON_R0HW | MCP45HVX1_TCON_R0A | MCP45HVX1_TCON_R0W);
    //write(config, 0x3C, MEM_TCON | COM_WRITE, 0);
    //write(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_WRITE, 0);
    //test(config, 0x3C, MCP45HVX1_MEM_TCON | MCP45HVX1_COM_READ);
    //test(config, 0x3C, MCP45HVX1_MEM_WIPER | MCP45HVX1_COM_READ);

    //test(config, 0x3C, MEM_WIPER | COM_READ);
    //test(config, 0x3D);
    //test(config, 0x3E);
    //test(config, 0x3F);
	return 0;
}