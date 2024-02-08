#pragma once
#include <stdint.h>


/*
When the R0HW bit is “0”:
• The P0A terminal is disconnected
• The P0W terminal is simultaneously connected to
the P0B terminal (see Figure 5-11)

R0HW: Resistor 0 Hardware Configuration Control bit
This bit forces Resistor 0 into the “shutdown” configuration of the Hardware pin
1 = Resistor 0 is NOT forced to the hardware pin “shutdown” configuration
0 = Resistor 0 is forced to the hardware pin “shutdown” configuration
*/
#define MCP45HVX1_TCON_R0HW (0x08)
/*
R0A: Resistor 0 Terminal A (P0A pin) Connect Control bit
This bit connects/disconnects the Resistor 0 Terminal A to the Resistor 0 Network
1 = P0A pin is connected to the Resistor 0 Network
0 = P0A pin is disconnected from the Resistor 0 Network
*/
#define MCP45HVX1_TCON_R0A  (0x04)
/*
R0W: Resistor 0 Wiper (P0W pin) Connect Control bit
This bit connects/disconnects the Resistor 0 Wiper to the Resistor 0 Network
1 = P0W pin is connected to the Resistor 0 Network
0 = P0W pin is disconnected from the Resistor 0 Network
*/
#define MCP45HVX1_TCON_R0W  (0x02)
/*
R0B: Resistor 0 Terminal B (P0B pin) Connect Control bit
This bit connects/disconnects the Resistor 0 Terminal B to the Resistor 0 Network
1 = P0B pin is connected to the Resistor 0 Network
0 = P0B pin is disconnected from the Resistor 0 Network
*/
#define MCP45HVX1_TCON_R0B  (0x01)

/*
Reserved. Forced to “1”
*/
#define MCP45HVX1_TCON_RESERVED 0xF0

/*
Allowed commands: Read, Write, Increment, Decrement
*/
#define MCP45HVX1_MEM_WIPER           (0x00)

/*
Allowed commands: Read, Write
*/
#define MCP45HVX1_MEM_TCON            (0x40)

#define MCP45HVX1_COM_WRITE           (0x00)
#define MCP45HVX1_COM_READ            (0x0C)
#define MCP45HVX1_COM_WIPERINC        (0x04)
#define MCP45HVX1_COM_WIPERDEC        (0x08)


char const * MCP45HVX1_REG_tostring(int a);