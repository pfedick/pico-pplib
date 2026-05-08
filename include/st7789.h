#ifndef ST7789_H
#define ST7789_H
#pragma once

#include "hardware/dma.h"
#include "hardware/spi.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "picopplib.h"
#include "picopplib-grafix.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

class ST7789
{
public:
    class Config
    {
    public:
        uint8_t pin_spi_dc;
        uint8_t pin_spi_cs;
        uint8_t pin_spi_rst;
        uint8_t pin_spi_sck;
        uint8_t pin_spi_data;
        uint32_t pin_spi_speed;
        uint8_t pin_spi_blk;

        spi_inst_t* spi_num;
    };

private:
    uint8_t* oled_dma[2];
    uint8_t current_buffer;
    size_t buffer_size;
    uint16_t my_width;
    uint16_t my_height;

    uint8_t spi_dc;
    uint8_t spi_cs;
    uint8_t spi_rst;
    uint8_t spi_sck;
    uint8_t spi_data;
    uint32_t spi_speed;
    uint8_t spi_blk;
    spi_inst_t* spi_num;

    volatile unsigned int dma_tx;
    dma_channel_config config;

    void write(const uint8_t cmd, const uint8_t* data, size_t len);
    void flush_dma(uint8_t* ptr, size_t len);
    void oled_init();

public:
    ST7789();
    ~ST7789();

    void init(uint16_t width, uint16_t height, const Config& config);
    uint8_t* get_buffer() const;
    inline uint16_t width() { return my_width; };
    inline uint16_t height() { return my_height; };

    void refresh();
    picopplib::Drawable getDrawable();
    void clear(picopplib::Color color = picopplib::Color(0, 0, 0));
};

#endif // ST7789_H