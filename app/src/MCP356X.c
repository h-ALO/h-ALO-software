#include "MCP356X.h"


char const * MCP356X_REG_tostring(int a)
{
	switch(a)
	{
	case MCP356X_REG_ADC_DATA    : return "MCP356X_REG_ADC_DATA    ";
	case MCP356X_REG_CFG_0       : return "MCP356X_REG_CFG_0       ";
	case MCP356X_REG_CFG_1       : return "MCP356X_REG_CFG_1       ";
	case MCP356X_REG_CFG_2       : return "MCP356X_REG_CFG_2       ";
	case MCP356X_REG_CFG_3       : return "MCP356X_REG_CFG_3       ";
	case MCP356X_REG_IRQ         : return "MCP356X_REG_IRQ         ";
	case MCP356X_REG_MUX         : return "MCP356X_REG_MUX         ";
	case MCP356X_REG_SCAN        : return "MCP356X_REG_SCAN        ";
	case MCP356X_REG_TIMER       : return "MCP356X_REG_TIMER       ";
	case MCP356X_REG_OFFSET_CAL  : return "MCP356X_REG_OFFSET_CAL  ";
	case MCP356X_REG_GAIN_CAL    : return "MCP356X_REG_GAIN_CAL    ";
	case MCP356X_RSV_REG_W_A     : return "MCP356X_RSV_REG_W_A     ";
	case MCP356X_REG_LOCK        : return "MCP356X_REG_LOCK        ";
	case MCP356X_RSV_REG         : return "MCP356X_RSV_REG         ";
	case MCP356X_REG_CRC_CFG     : return "MCP356X_REG_CRC_CFG     ";
	default:return "";
	}
}







/*
111 = Gain is x64 (x16 analog, x4 digital)
110 = Gain is x32 (x16 analog, x2 digital)
101 = Gain is x16
100 = Gain is x8
011 = Gain is x4
010 = Gain is x2
001 = Gain is x1 (default)
000 = Gain is x1/3
*/

int32_t MCP356X_raw_to_millivolt(int32_t raw, int32_t vref_mv, int32_t gain_reg)
{
	int32_t c = (MCP356X_CALC_COEF / vref_mv);
	switch(gain_reg)
	{
	case MCP356X_CFG_2_GAIN_X_64  : c *= 64; break;
	case MCP356X_CFG_2_GAIN_X_32  : c *= 32; break;
	case MCP356X_CFG_2_GAIN_X_16  : c *= 16; break;
	case MCP356X_CFG_2_GAIN_X_8   : c *= 8; break;
	case MCP356X_CFG_2_GAIN_X_4   : c *= 4; break;
	case MCP356X_CFG_2_GAIN_X_2   : c *= 2; break;
	case MCP356X_CFG_2_GAIN_X_1   : c *= 1; break;
	case MCP356X_CFG_2_GAIN_X_033 : c /= 3; break;
	}
	int32_t mv;
	mv = raw / c;
	return mv;
}

/*
In DATA_FORMAT[1:0] = 11, the output code is similar
to the one in DATA_FORMAT[1:0] = 10. The only differ-
ence resides in the four MSbs of the first byte, which
are no longer repeats of the Sign bit (SGN). They are
the Channel ID data (CH_ID[3:0]) that are defined in
Table 5-14. This CH_ID[3:0] word can be used to verify
that the right channel has been converted to SCAN
mode and can serve easy data retrieval and logging
(see Section 5.14 “SCAN Mode” for more details
about the SCAN mode). In MUX mode, this 4-bit word
is defaulted to ‘0000’ and does not vary with the
MUX[7:0] selection. This format is useful for 32-bit
MCU applications.
*/
void MCP356X_ADC_DATA_decode_11(uint8_t rx[5], int32_t * out_value, uint8_t * out_channel)
{
	(*out_channel) = (rx[1] >> 4) & 0x0F;
	uint8_t sign = rx[1] & 0x01;
	(*out_value) = (rx[2] << 16) | (rx[3] << 8) | (rx[4] << 0);
	if (sign != 0)
	{
		(*out_value) -= 16777215;
	}
}




uint32_t MCP356X_get_len(uint8_t reg)
{
	switch(reg)
	{
	case MCP356X_REG_ADC_DATA:
		return 4; // 32 bit
	case MCP356X_REG_CFG_0:
	case MCP356X_REG_CFG_1:
	case MCP356X_REG_CFG_2:
	case MCP356X_REG_CFG_3:
	case MCP356X_REG_IRQ:
	case MCP356X_REG_MUX:
		return 1; // 8 bit
	case MCP356X_REG_SCAN:
	case MCP356X_REG_TIMER:
	case MCP356X_REG_OFFSET_CAL:
	case MCP356X_REG_GAIN_CAL:
	case MCP356X_RSV_REG_W_A:
		return 3; // 24 bit
	case MCP356X_REG_C:
	case MCP356X_REG_LOCK:
		return 1; // 8 bit
	case MCP356X_RSV_REG:
	case MCP356X_REG_CRC_CFG:
		return 2; // 16 bit
	default:
		return 0;
		break;
	}
}


uint32_t MCP356X_get_value(uint8_t rx[5], uint8_t len)
{
	switch (len)
	{
	case 1: return (rx[1] << 0);
	case 2: return (rx[1] << 8) | (rx[2] << 0);
	case 3: return (rx[1] << 16) | (rx[2] << 8) | (rx[3] << 0);
	case 4: return (rx[1] << 24) | (rx[2] << 16) | (rx[3] << 8) | (rx[4] << 0);
	default: return 0;
	}
}


void MCP356X_set_value(uint8_t tx[5], uint8_t len, uint32_t value)
{
	switch (len)
	{
	case 1:
		tx[1] = ( value >> 0 ) & 0xFF;
		break;
	case 2:
		tx[1] = ( value >> 8 ) & 0xFF;
		tx[2] = ( value >> 0 ) & 0xFF;
		break;
	case 3:
		tx[1] = ( value >> 16 ) & 0xFF;
		tx[2] = ( value >> 8 ) & 0xFF;
		tx[3] = ( value >> 0 ) & 0xFF;
		break;
	case 4:
		tx[1] = ( value >> 24 ) & 0xFF;
		tx[2] = ( value >> 16 ) & 0xFF;
		tx[3] = ( value >> 8 ) & 0xFF;
		tx[4] = ( value >> 0 ) & 0xFF;
		break;
	default:
		tx[1] = 0;
		tx[2] = 0;
		tx[3] = 0;
		tx[4] = 0;
		break;
	}
}



