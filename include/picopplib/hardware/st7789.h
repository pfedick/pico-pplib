/*******************************************************************************
 * This file is part of "Patrick's Programming Library" for Raspberry Pico,
 * based on PPLib Version 7.
 * Web: https://github.com/pfedick/pico-pplib
 *******************************************************************************
 * Copyright (c) 2026, Patrick Fedick <patrick@pfp.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright notice, this
 *       list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#ifndef PICOPPLIB_HARDWARE_ST7789_H
#define PICOPPLIB_HARDWARE_ST7789_H
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
    static constexpr uint8_t UNUSED_PIN = 255;
    enum class SPIMode
    {
        Mode0 = 0, // CPOL=0, CPHA=0
        Mode1 = 1, // CPOL=0, CPHA=1
        Mode2 = 2, // CPOL=1, CPHA=0
        Mode3 = 3  // CPOL=1, CPHA=1
    };
    class Config
    {
    public:
        uint8_t pin_spi_dc;
        uint8_t pin_spi_cs;
        uint8_t pin_spi_rst;
        uint8_t pin_spi_sck;
        uint8_t pin_spi_data;
        uint32_t pin_spi_speed;
        uint8_t pin_spi_blk;               // can be UNUSED_PIN if not used
        SPIMode spi_mode = SPIMode::Mode0; // Default SPI mode
        spi_inst_t* spi_num;
    };

    enum class Orientation
    {
        Portrait,         // 0°
        Landscape,        // 90° im Uhrzeigersinn, Default
        InvertedPortrait, // 180°
        InvertedLandscape // 270° im Uhrzeigersinn
    };

private:
    uint8_t* oled_dma[2];
    uint8_t current_buffer;
    size_t buffer_size; // kann berechnet werden aus my_width*my_height*2
    uint16_t my_width;
    uint16_t my_height;

    uint8_t spi_dc;
    uint8_t spi_cs;
    uint8_t spi_rst;  // kann weg nach init
    uint8_t spi_sck;  // kann weg nach init
    uint8_t spi_data; // kann weg nach init
    uint32_t spi_speed;
    uint8_t spi_blk;
    SPIMode spi_mode;
    spi_inst_t* spi_num;

    volatile unsigned int dma_tx;
    dma_channel_config config; // kann weg nach init
    Orientation orientation;

    void write(const uint8_t cmd, const uint8_t* data, size_t len);
    void flush_dma(uint8_t* ptr, size_t len);
    void init_tft();
    void init_pwm();

public:
    ST7789();
    ~ST7789();

    void init(uint16_t width, uint16_t height, const Config& config, bool useDoubleBuffer = false);
    void setBrightness(uint8_t brightness);
    void setOrientation(Orientation o);
    uint8_t* get_buffer() const;
    inline uint16_t width() { return my_width; };
    inline uint16_t height() { return my_height; };

    void refresh();
    void sync();
    picopplib::Drawable getDrawable();
    void clear(picopplib::Color color = picopplib::Color(0, 0, 0));
};

#endif // PICOPPLIB_HARDWARE_ST7789_H