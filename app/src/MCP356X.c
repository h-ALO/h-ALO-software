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


Formula

ADC * VREF / (COEF * GAIN)



*/

int32_t MCP356X_raw_to_millivolt(int32_t raw, int32_t vref_mv, uint8_t gain_reg)
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


int64_t MCP356X_raw_to_volt(int32_t raw, int32_t vref, uint8_t gain_reg)
{
	//return MCP356X_raw_to_millivolt(raw, vref, gain_reg);
	int64_t numerator = (int64_t)raw * (int64_t)vref;
	int64_t denominator = MCP356X_CALC_COEF;
	switch(gain_reg)
	{
	case MCP356X_CFG_2_GAIN_X_64  : denominator *= 64; break;
	case MCP356X_CFG_2_GAIN_X_32  : denominator *= 32; break;
	case MCP356X_CFG_2_GAIN_X_16  : denominator *= 16; break;
	case MCP356X_CFG_2_GAIN_X_8   : denominator *= 8; break;
	case MCP356X_CFG_2_GAIN_X_4   : denominator *= 4; break;
	case MCP356X_CFG_2_GAIN_X_2   : denominator *= 2; break;
	case MCP356X_CFG_2_GAIN_X_1   : denominator *= 1; break;
	case MCP356X_CFG_2_GAIN_X_033 : denominator /= 3; break;
	}
	int64_t result = numerator / denominator;
	return result;
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



uint8_t MCP356X_get_scan_channel_gain(uint8_t channel)
{
	switch (channel)
	{
	case MCP356X_CH_OFFSET  : return MCP356X_CFG_2_GAIN_X_1;
	case MCP356X_CH_VCM     : return MCP356X_CFG_2_GAIN_X_1;
	case MCP356X_CH_AVDD    : return MCP356X_CFG_2_GAIN_X_033;
	case MCP356X_CH_TEMP    : return MCP356X_CFG_2_GAIN_X_1;
	case MCP356X_CH_DIFF_D  : return MCP356X_CFG_2_GAIN_X_1;
	case MCP356X_CH_DIFF_C  : return MCP356X_CFG_2_GAIN_X_1;
	case MCP356X_CH_DIFF_B  : return MCP356X_CFG_2_GAIN_X_1;
	case MCP356X_CH_DIFF_A  : return MCP356X_CFG_2_GAIN_X_1;
	case MCP356X_CH_CH7     : return MCP356X_CFG_2_GAIN_X_1;
	case MCP356X_CH_CH6     : return MCP356X_CFG_2_GAIN_X_1;
	case MCP356X_CH_CH5     : return MCP356X_CFG_2_GAIN_X_1;
	case MCP356X_CH_CH4     : return MCP356X_CFG_2_GAIN_X_1;
	case MCP356X_CH_CH3     : return MCP356X_CFG_2_GAIN_X_1;
	case MCP356X_CH_CH2     : return MCP356X_CFG_2_GAIN_X_1;
	case MCP356X_CH_CH1     : return MCP356X_CFG_2_GAIN_X_1;
	case MCP356X_CH_CH0     : return MCP356X_CFG_2_GAIN_X_1;
	default                 : return 0xFF;
	}
}





char const * MCP356X_MUX_POS_to_str(uint32_t value)
{
	switch(value & 0xF0)
	{
		case MCP356X_MUX_VIN_POS_VCM            : return "VCM           ";
		case MCP356X_MUX_VIN_POS_TEMP_M         : return "TEMP_M        ";
		case MCP356X_MUX_VIN_POS_TEMP_P         : return "TEMP_P        ";
		case MCP356X_MUX_VIN_POS_VREF_EXT_MINUS : return "VREF_EXT_MINUS";
		case MCP356X_MUX_VIN_POS_VREF_EXT_PLUS  : return "VREF_EXT_PLUS ";
		case MCP356X_MUX_VIN_POS_VREF_INT       : return "VREF_INT      ";
		case MCP356X_MUX_VIN_POS_AVDD           : return "AVDD          ";
		case MCP356X_MUX_VIN_POS_AGND           : return "AGND          ";
		case MCP356X_MUX_VIN_POS_CH7            : return "CH7           ";
		case MCP356X_MUX_VIN_POS_CH6            : return "CH6           ";
		case MCP356X_MUX_VIN_POS_CH5            : return "CH5           ";
		case MCP356X_MUX_VIN_POS_CH4            : return "CH4           ";
		case MCP356X_MUX_VIN_POS_CH3            : return "CH3           ";
		case MCP356X_MUX_VIN_POS_CH2            : return "CH2           ";
		case MCP356X_MUX_VIN_POS_CH1            : return "CH1           ";
		case MCP356X_MUX_VIN_POS_CH0            : return "CH0           ";
		default: return "";
	}
}

char const * MCP356X_MUX_NEG_to_str(uint32_t value)
{
	switch(value & 0x0F)
	{
		case MCP356X_MUX_VIN_NEG_VCM            : return "VCM           ";
		case MCP356X_MUX_VIN_NEG_TEMP_M         : return "TEMP_M        ";
		case MCP356X_MUX_VIN_NEG_TEMP_P         : return "TEMP_P        ";
		case MCP356X_MUX_VIN_NEG_VREF_EXT_MINUS : return "VREF_EXT_MINUS";
		case MCP356X_MUX_VIN_NEG_VREF_EXT_PLUS  : return "VREF_EXT_PLUS ";
		case MCP356X_MUX_VIN_NEG_VREF_INT       : return "VREF_INT      ";
		case MCP356X_MUX_VIN_NEG_AVDD           : return "AVDD          ";
		case MCP356X_MUX_VIN_NEG_AGND           : return "AGND          ";
		case MCP356X_MUX_VIN_NEG_CH7            : return "CH7           ";
		case MCP356X_MUX_VIN_NEG_CH6            : return "CH6           ";
		case MCP356X_MUX_VIN_NEG_CH5            : return "CH5           ";
		case MCP356X_MUX_VIN_NEG_CH4            : return "CH4           ";
		case MCP356X_MUX_VIN_NEG_CH3            : return "CH3           ";
		case MCP356X_MUX_VIN_NEG_CH2            : return "CH2           ";
		case MCP356X_MUX_VIN_NEG_CH1            : return "CH1           ";
		case MCP356X_MUX_VIN_NEG_CH0            : return "CH0           ";
		default: return "";
	}
}


// DS20006181C-page 34
// EQUATION 5-1: TEMPERATURE SENSOR TRANSFER FUNCTION
double MCP356X_ADCDATA_to_temperature_o1(int32_t adcdata)
{
	float ret = 0.00133 * adcdata - 267.146;
	return ret;
}

double MCP356X_ADCDATA_to_temperature_o3(int32_t adcdata)
{
	const double a3 = -3.904E-15;
	const double a2 = 3.814E-9;
	const double a1 = 0.0002;
	double ret = 
	a1 * (adcdata * adcdata * adcdata) + 
	a2 * (adcdata * adcdata) + 
	a3 * (adcdata) - 163.978;
	return ret;
}






// TABLE 5-14: ADC CHANNEL SELECTION
// DS20006181C-page 54
uint8_t MCP356X_channel_to_mux(uint8_t channel)
{
	switch (channel)
	{
	case MCP356X_CH_CH0: return MCP356X_MUX_VIN_NEG_AGND | MCP356X_MUX_VIN_POS_CH0;
	case MCP356X_CH_CH1: return MCP356X_MUX_VIN_NEG_AGND | MCP356X_MUX_VIN_POS_CH1;
	case MCP356X_CH_CH2: return MCP356X_MUX_VIN_NEG_AGND | MCP356X_MUX_VIN_POS_CH2;
	case MCP356X_CH_CH3: return MCP356X_MUX_VIN_NEG_AGND | MCP356X_MUX_VIN_POS_CH3;
	case MCP356X_CH_CH4: return MCP356X_MUX_VIN_NEG_AGND | MCP356X_MUX_VIN_POS_CH4;
	case MCP356X_CH_CH5: return MCP356X_MUX_VIN_NEG_AGND | MCP356X_MUX_VIN_POS_CH5;
	case MCP356X_CH_CH6: return MCP356X_MUX_VIN_NEG_AGND | MCP356X_MUX_VIN_POS_CH6;
	case MCP356X_CH_DIFF_A: return MCP356X_MUX_VIN_NEG_CH1 | MCP356X_MUX_VIN_POS_CH0;
	case MCP356X_CH_DIFF_B: return MCP356X_MUX_VIN_NEG_CH3 | MCP356X_MUX_VIN_POS_CH2;
	case MCP356X_CH_DIFF_C: return MCP356X_MUX_VIN_NEG_CH5 | MCP356X_MUX_VIN_POS_CH4;
	case MCP356X_CH_DIFF_D: return MCP356X_MUX_VIN_NEG_CH7 | MCP356X_MUX_VIN_POS_CH6;
	case MCP356X_CH_TEMP: return MCP356X_MUX_VIN_NEG_TEMP_M | MCP356X_MUX_VIN_POS_TEMP_P;
	case MCP356X_CH_AVDD: return MCP356X_MUX_VIN_NEG_AGND | MCP356X_MUX_VIN_POS_AVDD;
	case MCP356X_CH_VCM: return MCP356X_MUX_VIN_NEG_VCM | MCP356X_MUX_VIN_POS_CH0;
	case MCP356X_CH_OFFSET: return MCP356X_MUX_VIN_NEG_AGND | MCP356X_MUX_VIN_NEG_AGND;
	}
	return 0;
}