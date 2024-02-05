#include "MCP45HVX1.h"



char const * MCP45HVX1_REG_tostring(int a)
{
	switch(a)
	{
	case MCP45HVX1_MEM_WIPER : return "MEM_WIPER ";
	case MCP45HVX1_MEM_TCON  : return "MEM_TCON  ";
	default:return "";
	}
}