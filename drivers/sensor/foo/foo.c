#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <drivers/pinctrl.h>
#include <zephyr.h>
#include <device.h>
#include <devicetree.h>


#include <device.h>
#include <drivers/gpio.h>
#include <drivers/sensor.h>



#define DT_DRV_COMPAT zephyr_foo

struct spi_dt_spec spi_spec1 = SPI_DT_SPEC_GET(DT_NODELABEL(foo), SPI_WORD_SET(8) | SPI_MODE_GET(0), 1);