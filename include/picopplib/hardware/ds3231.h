#include "pico/time.h"
#include "pico/stdlib.h"
#include "stdlib.h"
#include "stdio.h"
#include <unistd.h>
#include "pico/types.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"

class DS3231
{
private:
    i2c_inst_t* i2c_port;
    int i2c_scl;
    int i2c_sda;
    int addr;

public:
    DS3231();
    DS3231(i2c_inst_t* i2c_port, int i2c_scl, int i2c_sda, int addr = 0x68);
    void init(i2c_inst_t* i2c_port, int i2c_scl, int i2c_sda, int addr = 0x68);

    void setTime(const datetime_t& time);
    void getTime(datetime_t& time);

    float getTemperature() const;
};
