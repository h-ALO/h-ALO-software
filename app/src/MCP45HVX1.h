#pragma once
#include <stdint.h>



#define MCP45HVX1_TCON_R0HW (0x08)  // Shutdown Resistor Force
#define MCP45HVX1_TCON_R0A  (0x04)  // Terminal A Connection 
#define MCP45HVX1_TCON_R0W  (0x02)  // Wiper Connection
#define MCP45HVX1_TCON_R0B  (0x01)  // Terminal B Connection

#define MCP45HVX1_GCALL_TCON          (0x60)
#define MCP45HVX1_GCALL_WIPER         (0x40)
#define MCP45HVX1_GCALL_WIPERUP       (0x42)
#define MCP45HVX1_GCALL_WIPERDWN      (0x44)
#define MCP45HVX1_GCALL_COM_WRITE     (0x02)
#define MCP45HVX1_GCALL_COM_RWRITE    (0x03)
#define MCP45HVX1_GCALL_COM_WIPERINC  (0x42)
#define MCP45HVX1_GCALL_COM_WIPERDEC  (0x44)

#define MCP45HVX1_MEM_WIPER           (0x00)
#define MCP45HVX1_MEM_TCON            (0x40)

#define MCP45HVX1_COM_WRITE           (0x00)
#define MCP45HVX1_COM_READ            (0x0C)
#define MCP45HVX1_COM_WIPERINC        (0x04)
#define MCP45HVX1_COM_WIPERDEC        (0x08)


char const * MCP45HVX1_REG_tostring(int a);