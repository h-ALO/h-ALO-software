#include "MCP45HVX1.h"
/*
https://ww1.microchip.com/downloads/en/DeviceDoc/20005304A.pdf
https://github.com/h-ALO/h-ALO-zephyr-app/blob/main/app/src/dpot.c
https://github.com/WifWaf/MCP45HVX1/blob/master/src/MCP45HVX1.cpp


S 0 1 1 1 1 A1 A0 rw A


*/


char const * MCP45HVX1_REG_tostring(int a)
{
	switch(a)
	{
	case MCP45HVX1_MEM_WIPER : return "MEM_WIPER ";
	case MCP45HVX1_MEM_TCON  : return "MEM_TCON  ";
	default:return "";
	}
}

