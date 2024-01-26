#include "dpot.h"


#define TCON_R0HW (0x08)  // Shutdown Resistor Force
#define TCON_R0A  (0x04)  // Terminal A Connection 
#define TCON_R0W  (0x02)  // Wiper Connection
#define TCON_R0B  (0x01)  // Terminal B Connection

#define GCALL_TCON          (0x60)
#define GCALL_WIPER         (0x40)
#define GCALL_WIPERUP       (0x42)
#define GCALL_WIPERDWN      (0x44)
#define GCALL_COM_WRITE     (0x02)
#define GCALL_COM_RWRITE    (0x03)
#define GCALL_COM_WIPERINC  (0x42)
#define GCALL_COM_WIPERDEC  (0x44)

#define MEM_WIPER           (0x00)
#define MEM_TCON            (0x40)

#define COM_WRITE           (0x00)
#define COM_READ            (0x0C)
#define COM_WIPERINC        (0x04)
#define COM_WIPERDEC        (0x08)

void write(struct mcp45hvx1_config * config, int addr, int cmd, int value)
{
	printk("write : %02x %02x %02x\n", addr, cmd, value);
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


	printk("\n\n");
}


void test(struct mcp45hvx1_config * config, int addr, int cmd)
{
	printk("Reading : %02x %02x\n", addr, cmd);
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

	// Print the read data
	printk("Read data from register: 0x%02X 0x%02X\n", read_data[0], read_data[1]);
    
	printk("\n\n");
}

int dpot_setup(struct mcp45hvx1_config * config)
{
    test(config, 0x3C, MEM_TCON | COM_READ);
    write(config, 0x3C, MEM_TCON | COM_WRITE, 0xF2);
    test(config, 0x3C, MEM_TCON | COM_READ);

    //test(config, 0x3C, MEM_WIPER | COM_READ);
    //test(config, 0x3D);
    //test(config, 0x3E);
    //test(config, 0x3F);
}