#pragma once
#include <stdint.h>

// https://github.com/MikroElektronika/mikrosdk_click_v2
// https://github.com/MikroElektronika/mikrosdk_click_v2/tree/master/clicks/adc9
// https://github.com/jspark311/MCP356x-Breakout/blob/6cc57254ad991eba59b2607941b86dc148f06eaf/Arduino/src/MCP356x.cpp






/*****************************************************************
6.2.1 COMMAND BYTE STRUCTURE
The COMMAND byte fully defines the command that
will be executed by the part. This byte is divided into
three parts: the device address bits (CMD[7:6]), the
command address bits (CMD[5:2]) and the
command-type bits (CMD[1:0]).
*****************************************************************/
#define MCP356X_COMMAND_BYTE(addr, cmd, type) (((addr)<<6) | ((cmd)<<2) | ((type)<<0))

/*****************************************************************
6.2.2 DEVICE ADDRESS BITS (CMD[7:6])
The SPI interface of the MCP3561/2/4 devices is
addressable, which means that multiple devices can
communicate on the same SPI bus with only one Chip
Select line for all devices. Each device communication
starts by a CS falling edge, followed by the clocking of
the device address (CMD[7:6]). Each device contains
an internal device address which the device can
respond to.
This address is coded on two bits, so four possible
addresses are available. Device address is hard-coded
within the device and should be determined when
ordering the device. The device address is part of the
device markings to avoid potential confusion (see
Sections 9.1 “Package Marking Information(1)”).
When the CMD[7:6] bits match the device address, the
communication will proceed and the part will execute
the commands defined in the control byte and its
subsequent data bytes.
When the CMD[7:6] bits do not correspond to the
address hard-coded in the device, the command is
ignored. In this case, the SDO output will become
high-impedance, which prevents bus contention errors
when multiple devices are connected on the same SPI
bus (see Figure 6-2). The user has to exit from this
communication through a CS rising edge to be able to
launch another command.
*****************************************************************/
#define MCP356X_DEVICE_ADR                   0x01 // CMD[7:6]

/*****************************************************************
6.2.4 COMMAND-TYPE BITS (CMD[1:0])
The last two bits of the COMMAND register byte define
the command type. These bits are an extension of the
typical read/write bits present in most SPI communica-
tion protocols. The two bits define four possible
command types: Incremental Write, Incremental Read,
Static Read and Fast command. Changing the com-
mand type within the same communication (while CS is
logic low) is not possible. The communication has to be
stopped (CS rising edge) and restarted (CS falling
edge) to change its command type.
*****************************************************************/
#define MCP356X_CMD_DONT_CARE                0x00
#define MCP356X_CMD_STAT_READ                0x01
#define MCP356X_CMD_INC_WRITE                0x02
#define MCP356X_CMD_INC_READ                 0x03

/*****************************************************************
8.0 INTERNAL REGISTERS
The MCP3561/2/4 devices have a total of 16 internal
registers made of volatile memory.
*****************************************************************/

/*
8.1 ADCDATA Register
The data can be
formatted in 24/32-bit modes depending on the DATA_FORMAT
*/
#define MCP356X_REG_ADC_DATA                 0x00 // | 4/24/32 | R  | 
#define MCP356X_REG_CFG_0                    0x01 // | 8       | RW | 
#define MCP356X_REG_CFG_1                    0x02 // | 8       | RW | 
#define MCP356X_REG_CFG_2                    0x03 // | 8       | RW | 
#define MCP356X_REG_CFG_3                    0x04 // | 8       | RW | 
#define MCP356X_REG_IRQ                      0x05 // | 8       | RW | 
#define MCP356X_REG_MUX                      0x06 // | 8       | RW | 
#define MCP356X_REG_SCAN                     0x07 // | 24      | RW | 
#define MCP356X_REG_TIMER                    0x08 // | 24      | RW | 
#define MCP356X_REG_OFFSET_CAL               0x09 // | 24      | RW | 
#define MCP356X_REG_GAIN_CAL                 0x0A // | 24      | RW | 
#define MCP356X_RSV_REG_W_A                  0x0B // | 24      | RW | 
#define MCP356X_REG_C                        0x0C // | 8       | RW | 
#define MCP356X_REG_LOCK                     0x0D // | 8       | RW | 
#define MCP356X_RSV_REG                      0x0E // | 16      | RW | 
#define MCP356X_REG_CRC_CFG                  0x0F // | 16      | R  | 



/*
Address Register Name No. of
Bits R/W Description
0x0 ADCDATA 4/24/32 R Latest A/D conversion data output value (24 or 32 bits depending on
DATA_FORMAT[1:0]) or modulator output stream (4-bit wide) in MDAT
Output mode
0x1 CONFIG0 8 R/W ADC Operating mode, Master Clock mode and Input Bias Current
Source mode
0x2 CONFIG1 8 R/W Prescale and OSR settings
0x3 CONFIG2 8 R/W ADC boost and gain settings, auto-zeroing settings for analog
multiplexer, voltage reference and ADC
0x4 CONFIG3 8 R/W Conversion mode, data and CRC format settings; enable for CRC on
communications, enable for digital offset and gain error calibrations
0x5 IRQ 8 R/W IRQ Status bits and IRQ mode settings; enable for Fast commands and
for conversion start pulse
0x6 MUX 8 R/W Analog multiplexer input selection (MUX mode only)
0x7 SCAN 24 R/W SCAN mode settings
0x8 TIMER 24 R/W Delay value for TIMER between scan cycles
0x9 OFFSETCAL 24 R/W ADC digital offset calibration value
0xA GAINCAL 24 R/W ADC digital gain calibration value
0xB RESERVED 24 R/W
0xC RESERVED 8 R/W
0xD LOCK 8 R/W Password value for SPI Write mode locking
0xE RESERVED 16 R/W
0xF CRCCFG 16 R CRC checksum for device configuration
*/





















/*****************************************************************
8.2 CONFIG0 Register
ADC Operating mode, Master Clock mode and Input Bias Current
Source mode
******************************************************************/
// Full Shutdown Mode Enable
#define MCP356X_CFG_0_VREF_SEL_0             0x00
#define MCP356X_CFG_0_VREF_SEL_1             0x40
#define MCP356X_CFG_0_VREF_SEL_2             0x80
#define MCP356X_CFG_0_VREF_SEL_3             0xC0
// Clock Selection
#define MCP356X_CFG_0_CLK_SEL_0              0x00
#define MCP356X_CFG_0_CLK_SEL_1              0x10
#define MCP356X_CFG_0_CLK_SEL_2              0x20
#define MCP356X_CFG_0_CLK_SEL_3              0x30
// Current Source/Sink Selection Bits for Sensor Bias (source on VIN+/sink on VIN-)
#define MCP356X_CFG_0_CS_SEL_0               0x00
#define MCP356X_CFG_0_CS_SEL_1               0x04
#define MCP356X_CFG_0_CS_SEL_2               0x08
#define MCP356X_CFG_0_CS_SEL_3               0x0C
// ADC Operating Mode Selection
#define MCP356X_CFG_0_MODE_SHD_DEF           0x00
#define MCP356X_CFG_0_MODE_SHD               0x01
#define MCP356X_CFG_0_MODE_STANDBY           0x02
#define MCP356X_CFG_0_MODE_CONV              0x03




/******************************************************************
8.3 CONFIG1 Register
Prescale and OSR settings
******************************************************************/
// Prescaler Value Selection for AMCLK
#define MCP356X_CFG_1_PRE_1                  0x00
#define MCP356X_CFG_1_PRE_2                  0x40
#define MCP356X_CFG_1_PRE_4                  0x80
#define MCP356X_CFG_1_PRE_8                  0xC0
// Oversampling Ratio for Delta-Sigma A/D Conversion
#define MCP356X_CFG_1_OSR_98304              0x3C
#define MCP356X_CFG_1_OSR_81920              0x38
#define MCP356X_CFG_1_OSR_49152              0x34
#define MCP356X_CFG_1_OSR_40960              0x30
#define MCP356X_CFG_1_OSR_24576              0x2C
#define MCP356X_CFG_1_OSR_20480              0x28
#define MCP356X_CFG_1_OSR_16384              0x24
#define MCP356X_CFG_1_OSR_8192               0x20
#define MCP356X_CFG_1_OSR_4096               0x1C
#define MCP356X_CFG_1_OSR_2048               0x18
#define MCP356X_CFG_1_OSR_1024               0x14
#define MCP356X_CFG_1_OSR_512                0x10
#define MCP356X_CFG_1_OSR_256                0x0C
#define MCP356X_CFG_1_OSR_128                0x08
#define MCP356X_CFG_1_OSR_64                 0x04
#define MCP356X_CFG_1_OSR_32                 0x00
// RESERVED[1:0]: Should always be set to ‘00’
#define MCP356X_CFG_1_DITHER_MAX             0x03
#define MCP356X_CFG_1_DITHER_MED             0x02
#define MCP356X_CFG_1_DITHER_MIN             0x01
#define MCP356X_CFG_1_DITHER_DEF             0x00





/******************************************************************
8.4 CONFIG2 Register
ADC boost and gain settings, auto-zeroing settings for analog
multiplexer, voltage reference and ADC
******************************************************************/
// ADC Bias Current Selection
#define MCP356X_CFG_2_BOOST_X_2              0xC0
#define MCP356X_CFG_2_BOOST_X_1              0x80
#define MCP356X_CFG_2_BOOST_X_066            0x40
#define MCP356X_CFG_2_BOOST_X_05             0x00
/*
5.3.1
ANALOG GAIN
The gain settings from 0.33x to 16x are done in the
analog domain. This analog gain is placed on each ADC
differential input. Each doubling of the gain improves the
thermal noise due to sampling by approximately 3 dB,
which means the lowest noise configuration is obtained
when using the highest analog gain. The SNR, however,
is degraded, since doubling the gain factor reduces the
maximum allowable input signal amplitude by
approximately 6 dB.
If the gain is set to 0.33x, the differential input range
theoretically becomes ±3 * VREF. However, the device
does not support input voltages outside of the power
supply voltage range. If large reference voltages are
used with this gain, the input voltage range will be
clipped between AGND and AVDD, and therefore, the
output code span will be limited. This gain is useful
when the reference voltage is small and when the
input signal voltage is large.
The analog gain stage can be used to amplify very low
signals, but the differential input range of the
Delta-Sigma modulator must not be exceeded.
*/
#define MCP356X_CFG_2_GAIN_X_64              0x38
#define MCP356X_CFG_2_GAIN_X_32              0x30
#define MCP356X_CFG_2_GAIN_X_16              0x28
#define MCP356X_CFG_2_GAIN_X_8               0x20
#define MCP356X_CFG_2_GAIN_X_4               0x18
#define MCP356X_CFG_2_GAIN_X_2               0x10
#define MCP356X_CFG_2_GAIN_X_1               0x08
#define MCP356X_CFG_2_GAIN_X_033             0x00
// Auto-Zeroing MUX Setting
#define MCP356X_CFG_2_AZ_MUX_EN              0x04
#define MCP356X_CFG_2_AZ_MUX_DIS             0x00
#define MCP356X_CFG_2_AZ_VREF_EN             0x02
#define MCP356X_CFG_2_AZ_VREF_DIS            0x00
// RESERVED[1:0]: Should always be equal to ‘11’
#define MCP356X_CFG_2_AZ_FREQ_HIGH           0x01
#define MCP356X_CFG_2_AZ_FREQ_LOW            0x00




/******************************************************************
8.5 CONFIG3 Register
Conversion mode, data and CRC format settings; enable for CRC on
communications, enable for digital offset and gain error calibrations
*******************************************************************/
// Conversion Mode Selection
#define MCP356X_CFG_3_CONV_MODE_CONT         0xC0
#define MCP356X_CFG_3_CONV_MODE_STANDBY      0x80
#define MCP356X_CFG_3_CONV_MODE_SHD          0x40
#define MCP356X_CFG_3_CONV_MODE_SHD0         0x00

// ADC Output Data Format Selection

// 32-bit (25-bit right justified data + Channel ID): CHID[3:0] + SGN extension (4 bits) + 24-bit ADC data. 
// It allows overrange with the SGN extension.
#define MCP356X_CFG_3_DATA_FORMAT_CH_ADC     0x30
// 32-bit (25-bit right justified data): SGN extension (8-bit) + 24-bit ADC data. It allows overrange with the SGN extension.
#define MCP356X_CFG_3_DATA_FORMAT_SIGN_ADC   0x20
// 32-bit (24-bit left justified data): 24-bit ADC data + 0x00 (8-bit). It does not allow overrange (ADC code locked to 0xFFFFFF or 0x800000).
#define MCP356X_CFG_3_DATA_FORMAT_LEFT_JUST  0x10
//24-bit (default ADC coding): 24-bit ADC data. It does not allow overrange (ADC code locked to 0xFFFFFF or 0x800000).
#define MCP356X_CFG_3_DATA_FORMAT_DEF        0x00

// CRC Checksum Format Selection on Read Communications
#define MCP356X_CFG_3_CRC_FORMAT_32          0x08
#define MCP356X_CFG_3_CRC_FORMAT_16          0x00
// CRC Checksum Selection on Read Communications (it does not affect CRCCFG calculations)
#define MCP356X_CFG_3_CRC_COM_EN             0x04
#define MCP356X_CFG_3_CRC_COM_DIS            0x00
// Enable Digital Offset Calibration
#define MCP356X_CFG_3_CRC_OFF_CAL_EN         0x02
#define MCP356X_CFG_3_CRC_OFF_CAL_DIS        0x00
// Enable Digital Gain Calibration
#define MCP356X_CFG_3_CRC_GAIN_CAL_EN        0x01
#define MCP356X_CFG_3_CRC_GAIN_CAL_DIS       0x00



/*******************************************************************
8.6 IRQ Register
IRQ Status bits and IRQ mode settings; enable for Fast commands and
for conversion start pulse
*******************************************************************/
#define MCP356X_IRQ_MODE_MDAT                0x08
#define MCP356X_IRQ_MODE_IRQ                 0x00
#define MCP356X_IRQ_MODE_LOGIC_HIGH          0x04
#define MCP356X_IRQ_MODE_HIGH_Z              0x00
#define MCP356X_IRQ_FASTCMD_EN               0x02
#define MCP356X_IRQ_FASTCMD_DIS              0x00
#define MCP356X_IRQ_STP_EN                   0x01
#define MCP356X_IRQ_STP_DIS                  0x00

#define MCP356X_IRQ_MASK_EN_STP        0x01
#define MCP356X_IRQ_MASK_EN_FASTCMD    0x02
#define MCP356X_IRQ_MASK_MODE0         0x04
#define MCP356X_IRQ_MASK_MODE1         0x08
#define MCP356X_IRQ_MASK_POR_STATUS    0x10
#define MCP356X_IRQ_MASK_CRCCFG_STATUS 0x20
#define MCP356X_IRQ_MASK_DR_STATUS     0x40
#define MCP356X_IRQ_MASK_UNIMPLEMENTED 0x80



/*******************************************************************
8.7 MULTIPLEXER (MUX) REGISTER
Analog multiplexer input selection (MUX mode only)
*******************************************************************/
// Bit 7-4 MUX_VIN+ Input Selection
#define MCP356X_MUX_VIN_POS_NO_IN            0xF0 // 1111 0000 Internal VCM
#define MCP356X_MUX_VIN_POS_VCM              0xE0 // 1110 0000 Internal Temperature Sensor Diode M (Temp Diode M)(1)
#define MCP356X_MUX_VIN_POS_TEMP             0xD0 // 1101 0000 Internal Temperature Sensor Diode P (Temp Diode P)(1)
#define MCP356X_MUX_VIN_POS_VREF_EXT_MINUS   0xC0 // 1100 0000
#define MCP356X_MUX_VIN_POS_VREF_EXT_PLUS    0xB0 // 1011 0000
#define MCP356X_MUX_VIN_POS_VREF_INT         0xA0 // 1010 0000 Reserved (do not use)
#define MCP356X_MUX_VIN_POS_AVDD             0x90 // 1001 0000
#define MCP356X_MUX_VIN_POS_AGND             0x80 // 1000 0000
#define MCP356X_MUX_VIN_POS_CH7              0x70 // 0111 0000
#define MCP356X_MUX_VIN_POS_CH6              0x60 // 0110 0000
#define MCP356X_MUX_VIN_POS_CH5              0x50 // 0101 0000
#define MCP356X_MUX_VIN_POS_CH4              0x40 // 0100 0000
#define MCP356X_MUX_VIN_POS_CH3              0x30 // 0011 0000
#define MCP356X_MUX_VIN_POS_CH2              0x20 // 0010 0000
#define MCP356X_MUX_VIN_POS_CH1              0x10 // 0001 0000
#define MCP356X_MUX_VIN_POS_CH0              0x00 // 0000 0000
// Bit 3-0 MUX_VIN- Input Selection
#define MCP356X_MUX_VIN_NEG_NO_IN            0x0F // 0000 1111
#define MCP356X_MUX_VIN_NEG_VCM              0x0E // 0000 1110 Internal Temperature Sensor Diode M (Temp Diode M)(1)
#define MCP356X_MUX_VIN_NEG_TEMP             0x0D // 0000 1101 Internal Temperature Sensor Diode P (Temp Diode P)(1)
#define MCP356X_MUX_VIN_NEG_VREF_EXT_MINUS   0x0C // 0000 1100
#define MCP356X_MUX_VIN_NEG_VREF_EXT_PLUS    0x0B // 0000 1011
#define MCP356X_MUX_VIN_NEG_VREF_INT         0x0A // 0000 1010 Reserved (do not use)
#define MCP356X_MUX_VIN_NEG_AVDD             0x09 // 0000 1001
#define MCP356X_MUX_VIN_NEG_AGND             0x08 // 0000 1000
#define MCP356X_MUX_VIN_NEG_CH7              0x07 // 0000 0111
#define MCP356X_MUX_VIN_NEG_CH6              0x06 // 0000 0110
#define MCP356X_MUX_VIN_NEG_CH5              0x05 // 0000 0101
#define MCP356X_MUX_VIN_NEG_CH4              0x04 // 0000 0100
#define MCP356X_MUX_VIN_NEG_CH3              0x03 // 0000 0011
#define MCP356X_MUX_VIN_NEG_CH2              0x02 // 0000 0010
#define MCP356X_MUX_VIN_NEG_CH1              0x01 // 0000 0001
#define MCP356X_MUX_VIN_NEG_CH0              0x00 // 0000 0000




/* 8.8 SCAN Register
Bit 23-21 DLY[2:0]: Delay Time (TDLY_SCAN) Between Each Conversion During a Scan Cycle

When in SCAN mode, the MUX register (address: 0x6)
becomes a Don’t Care register.

When SCAN[15:0] = 0x0000, SCAN mode is dis-
abled and the part returns to MUX mode, where the
input channel selection is defined by the MUX[7:0] bits.
*/
#define MCP356X_SCAN_DLY_DM_CLK_X_512        0x00E00000
#define MCP356X_SCAN_DLY_DM_CLK_X_256        0x00C00000
#define MCP356X_SCAN_DLY_DM_CLK_X_128        0x00A00000
#define MCP356X_SCAN_DLY_DM_CLK_X_64         0x00800000
#define MCP356X_SCAN_DLY_DM_CLK_X_32         0x00600000
#define MCP356X_SCAN_DLY_DM_CLK_X_16         0x00400000
#define MCP356X_SCAN_DLY_DM_CLK_X_8          0x00200000
#define MCP356X_SCAN_DLY_NO_DELAY            0x00000000
//Bit 19-16 Unimplemented: Read as ‘0’
#define MCP356X_SCAN_PSAV_VREF_OFF           0x00100000
#define MCP356X_SCAN_PSAV_VREF_ON            0x00000000
// Bit 15-0 Scan Channel Selection (see Table 5-14 for a complete description)
#define MCP356X_SCAN_OFFSET                  0x00008000
#define MCP356X_SCAN_VREF                    0x00004000
#define MCP356X_SCAN_AVDD                    0x00002000
#define MCP356X_SCAN_TEMP                    0x00001000
#define MCP356X_SCAN_DIFF_D                  0x00000800
#define MCP356X_SCAN_DIFF_C                  0x00000400
#define MCP356X_SCAN_DIFF_B                  0x00000200
#define MCP356X_SCAN_DIFF_A                  0x00000100
#define MCP356X_SCAN_CH7                     0x00000080
#define MCP356X_SCAN_CH6                     0x00000040
#define MCP356X_SCAN_CH5                     0x00000020
#define MCP356X_SCAN_CH4                     0x00000010
#define MCP356X_SCAN_CH3                     0x00000008
#define MCP356X_SCAN_CH2                     0x00000004
#define MCP356X_SCAN_CH1                     0x00000002
#define MCP356X_SCAN_CH0                     0x00000001


/*
5.14.3.4 VCM Reading (V CM )
During the conversion that reads VCM , the device
monitors the internal Common-mode voltage of the
device in order to ensure proper operation.
The VCM voltage of the device should be located at
1.2V ± 2% to ensure proper accuracy. With this setting,
the internal multiplexer setting becomes 0xF8
(VCM – AGND). In order to properly measure VCM , the
reference voltage must be larger than 1.2V.
During the VCM reading, the gain of the ADC is set to 1x
regardless of the GAIN[2:0] settings. This temporary inter-
nal configuration does not change the register setting, it
impacts the gain of the device during this conversion.
The VCM reading is susceptible to the gain and offset
errors of the ADC, which should be calibrated to obtain
a precise internal Common-mode measurement.

Differential Channel D (CH6-CH7) 1011 0x67 None
10 Differential Channel C (CH4-CH5) 1010 0x45 None
9 Differential Channel B (CH2-CH3) 1001 0x23 None
8 Differential Channel A (CH0-CH1)
*/

#define MCP356X_CHANNEL_COUNT 16
#define MCP356X_CH_OFFSET   15
#define MCP356X_CH_VCM     14
#define MCP356X_CH_AVDD     13
#define MCP356X_CH_TEMP     12
#define MCP356X_CH_DIFF_D   11 // Differential Channel D (CH6-CH7)
#define MCP356X_CH_DIFF_C   10 // Differential Channel C (CH4-CH5)
#define MCP356X_CH_DIFF_B   9 // Differential Channel B (CH2-CH3)
#define MCP356X_CH_DIFF_A   8 // Differential Channel A (CH0-CH1)
#define MCP356X_CH_CH7      7
#define MCP356X_CH_CH6      6
#define MCP356X_CH_CH5      5
#define MCP356X_CH_CH4      4
#define MCP356X_CH_CH3      3
#define MCP356X_CH_CH2      2
#define MCP356X_CH_CH1      1
#define MCP356X_CH_CH0      0



// 8.9 TIMER Register
#define MCP356X_TIMER_DLY_DMCLK_X_16777215   0x00FFFFFF
#define MCP356X_TIMER_DLY_DMCLK_X_8388607    0x007FFFFF
#define MCP356X_TIMER_DLY_DMCLK_X_4194303    0x003FFFFF
#define MCP356X_TIMER_DLY_DMCLK_X_2097151    0x001FFFFF
#define MCP356X_TIMER_DLY_DMCLK_X_1048575    0x000FFFFF
#define MCP356X_TIMER_DLY_DMCLK_X_524287     0x0007FFFF
#define MCP356X_TIMER_DLY_DMCLK_X_262143     0x0003FFFF
#define MCP356X_TIMER_DLY_DMCLK_X_131071     0x0001FFFF
#define MCP356X_TIMER_DLY_DMCLK_X_65535      0x0000FFFF
#define MCP356X_TIMER_DLY_DMCLK_X_32767      0x00007FFF
#define MCP356X_TIMER_DLY_DMCLK_X_16383      0x00003FFF
#define MCP356X_TIMER_DLY_DMCLK_X_8191       0x00001FFF
#define MCP356X_TIMER_DLY_DMCLK_X_4095       0x00000FFF
#define MCP356X_TIMER_DLY_DMCLK_X_2047       0x000007FF
#define MCP356X_TIMER_DLY_DMCLK_X_1023       0x000003FF
#define MCP356X_TIMER_DLY_DMCLK_X_511        0x000001FF
#define MCP356X_TIMER_DLY_DMCLK_X_255        0x000000FF
#define MCP356X_TIMER_DLY_DMCLK_X_127        0x0000007F
#define MCP356X_TIMER_DLY_DMCLK_X_63         0x0000003F
#define MCP356X_TIMER_DLY_DMCLK_X_31         0x0000001F
#define MCP356X_TIMER_DLY_DMCLK_X_15         0x0000000F
#define MCP356X_TIMER_DLY_DMCLK_X_2          0x00000002
#define MCP356X_TIMER_DLY_DMCLK_X_1          0x00000001
#define MCP356X_TIMER_DLY_NO_DELAY           0x00000000

#define MCP356X_FAST_CMD_ADC_CONV_START      0x28
#define MCP356X_FAST_CMD_ADC_STANDBY_MODE    0x2C
#define MCP356X_FAST_CMD_ADC_SHUTDOWN_MODE   0x30
#define MCP356X_FAST_CMD_FULL_SHUTDOWN_MODE  0x34
#define MCP356X_FAST_CMD_DEV_FULL_RESET      0x38

#define MCP356X_CALC_COEF                    8388608


#define MCP356X_PRINTF_HEADER "OFFST VCM   AVDD  TEMP  DIFFD DIFFC DIFFB DIFFA CH7   CH6   CH5   CH4   CH3   CH2   CH1   CH0"
#define MCP356X_PRINTF_PLUS   "%+05i %+05i %+05i %+05i %+05i %+05i %+05i %+05i %+05i %+05i %+05i %+05i %+05i %+05i %+05i %+05i"
#define MCP356X_PRINTF "%05i %05i %05i %05i %05i %05i %05i %05i %05i %05i %05i %05i %05i %05i %05i %05i"
#define MCP356X_ARGS(x) \
(x)[MCP356X_CH_OFFSET],  \
(x)[MCP356X_CH_VCM],  \
(x)[MCP356X_CH_AVDD],  \
(x)[MCP356X_CH_TEMP],  \
(x)[MCP356X_CH_DIFF_D],  \
(x)[MCP356X_CH_DIFF_C],  \
(x)[MCP356X_CH_DIFF_B],  \
(x)[MCP356X_CH_DIFF_A],  \
(x)[MCP356X_CH_CH7],  \
(x)[MCP356X_CH_CH6],  \
(x)[MCP356X_CH_CH5],  \
(x)[MCP356X_CH_CH4],  \
(x)[MCP356X_CH_CH3],  \
(x)[MCP356X_CH_CH2],  \
(x)[MCP356X_CH_CH1],  \
(x)[MCP356X_CH_CH0]



char const * MCP356X_REG_tostring(int a);
int32_t MCP356X_raw_to_millivolt(int32_t raw, int32_t vref_mv, uint8_t gain_reg);
void MCP356X_ADC_DATA_decode_11(uint8_t rx[5], int32_t * out_value, uint8_t * out_channel);
uint32_t MCP356X_get_len(uint8_t reg);
uint32_t MCP356X_get_value(uint8_t rx[5], uint8_t len);
void MCP356X_set_value(uint8_t tx[5], uint8_t len, uint32_t value);
uint8_t MCP356X_get_scan_channel_gain(uint8_t channel);


