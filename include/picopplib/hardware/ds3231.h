#include "pico/time.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "pico/types.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
// #include "pico/aon_timer.h"

namespace picopplib
{
typedef struct
{
    int16_t year; // 0..4095
    int8_t month; // 1..12 (1 = Januar)
    int8_t day;   // 1..31 (je nach Monat)
    int8_t dotw;  // 0..6  (0 = Sonntag)
    int8_t hour;  // 0..23
    int8_t min;   // 0..59
    int8_t sec;   // 0..59
} datetime_t;

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

    datetime_t getTime() const;
    void setTime(const datetime_t& time);
    void setTime(const struct tm& time);

    void getTime(struct tm& time) const;
    void syncTimeToSystem();
    float getTemperature() const;
};

} // namespace picopplib