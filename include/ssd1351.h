#ifndef SSD1351_H
#define SSD1351_H
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

typedef enum ssd_commands
{
    SSD_CMD_SETCOLUMN = 0x15,
    SSD_CMD_WRITERAM = 0x5C,
    SSD_CMD_READRAM = 0x5D,
    SSD_CMD_SETROW = 0x75,
    SSD_CMD_SCROLL_SETUP = 0x96,
    SSD_CMD_SCROLL_START = 0x9E,
    SSD_CMD_SCROLL_STOP = 0x9F,
    SSD_CMD_SETREMAP = 0xA0,
    SSD_CMD_STARTLINE = 0xA1,
    SSD_CMD_DISPLAYOFFSET = 0xA2,
    SSD_CMD_PIXELSOFF = 0xA4, // All off
    SSD_CMD_PIXELSON = 0xA5,  // All on
    SSD_CMD_NONINVERT = 0xA6,
    SSD_CMD_INVERT = 0xA7,
    SSD_CMD_FUNCTIONSEL = 0xAB,
    SSD_CMD_DISPLAYOFF = 0xAE,
    SSD_CMD_DISPLAYON = 0xAF,
    SSD_CMD_PRECHARGE = 0xB1,
    SSD_CMD_ENHANCE = 0xB2,
    SSD_CMD_CLOCKDIV = 0xB3,
    SSD_CMD_SETVSL = 0xB4,
    SSD_CMD_SETGPIO = 0xB5,
    SSD_CMD_PRECHARGE2 = 0xB6,
    SSD_CMD_GAMMA_TABLE = 0xB8,
    SSD_CMD_RESET_GAMMA = 0xB9,
    SSD_CMD_PRECHARGEV = 0xBB,
    SSD_CMD_VCOMH = 0xBE,
    SSD_CMD_CONTRASTABC = 0xC1,
    SSD_CMD_CONTRASTMASTER = 0xC7,
    SSD_CMD_MUXRATIO = 0xCA,
    SSD_CMD_COMMANDLOCK = 0xFD
} ssd_commands;

class SSD1351
{
public:
    class Config
    {
    public:
        int pin_spi_dc;
        int pin_spi_rst;
        int pin_spi_sck;
        int pin_spi_data;
        int pin_spi_speed;
        spi_inst_t* spi_num;
    };

private:
    uint8_t* oled_dma[2];
    uint8_t current_buffer;
    size_t buffer_size;
    int width;
    int height;
    int spi_dc;
    int spi_rst;
    int spi_sck;
    int spi_data;
    int spi_speed;
    spi_inst_t* spi_num;
    uint8_t pos_x = 0;
    uint8_t pos_y = 0;

    volatile unsigned int dma_tx;
    dma_channel_config config;

    void write(const uint8_t cmd, const uint8_t* data, size_t len);
    void flush_dma(uint8_t* ptr, size_t len);
    void oled_init();

public:
    SSD1351();
    ~SSD1351();

    void init(int width, int height, const Config& config);

    uint8_t* get_buffer();

    void refresh();
    picopplib::Drawable getDrawable();

    void clear(uint16_t color = 0x0000);
    void putPixel(int x, int y, uint16_t color);
    void fillRect(int x1, int y1, int x2, int y2, uint16_t color);
    void drawRect(int x1, int y1, int x2, int y2, uint16_t color);
    void line(int x1, int y1, int x2, int y2, uint16_t color);
};

#endif