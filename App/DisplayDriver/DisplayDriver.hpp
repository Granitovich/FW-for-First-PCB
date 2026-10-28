#pragma once
#include "main.h"

#include "Constants/Fonts.h"
#include "Constants/Images.h"


/*
   DisplayDriver.cpp
 * Copyright (C) 2026 Anatoliy Lizanets, Andrew Kushyk, Andriy Honcharenko, ScarsFun
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

namespace DriverLayer
{
    class DisplayDriver
    {
    public:
        enum class DisplayStatus
        {
            OK,
            InvalidParameter,
            SendError
        };

        typedef struct
        {
            GPIO_TypeDef *resetPort;
            uint16_t resetPin;
            GPIO_TypeDef *dataCmdPort;
            uint16_t dataCmdPin;
            GPIO_TypeDef *csPort;
            uint16_t csPin;
            GPIO_TypeDef *ledPort;
            uint16_t ledPin;
        } DriverGpio;

        explicit DisplayDriver(SPI_HandleTypeDef *hspi, DriverGpio &gpios);

        DisplayStatus init();
        DisplayStatus setRotation(const uint8_t rotation);
        DisplayStatus drawPixel(const uint16_t x, const uint16_t y, const uint16_t color);
        DisplayStatus drawImage(const uint16_t* image);
        DisplayStatus drawLine (int16_t x0, int16_t y0, int16_t x1, int16_t y1, const uint16_t color);
        DisplayStatus drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
        DisplayStatus drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
        DisplayStatus fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
        DisplayStatus fillScreen(uint16_t color);
        DisplayStatus drawString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor);
        DisplayStatus drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
        DisplayStatus fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);



        //void drawString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor);
        void invertColors(bool invert);
        void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
        void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
        void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
        void drawEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color);
        void fillEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color);
        void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
        void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

        uint8_t getRotation(void);
        int16_t getHeight(void);
        int16_t getWidth(void);
        uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

        typedef enum : uint16_t
        {
            // Основні кольори
            BLACK       = 0x0000,
            NAVY        = 0x000F,
            DARKBLUE    = 0x0010,
            BLUE        = 0x001F,
            TEAL        = 0x0410,
            EMERALD     = 0x03E0,
            GREEN       = 0x07E0,
            CYAN        = 0x07FF,
            MAROON      = 0x8000,
            PURPLE      = 0x8010,
            INDIGO      = 0x4810,
            RED         = 0xF800,
            MAGENTA     = 0xF81F,
            YELLOW      = 0xFFE0,
            WHITE       = 0xFFFF,

            // Відтінки сірого (Grayscale / Gray) — від темного до світлого
            DARKER_GRAY = 0x2104, // Дуже темний сірий
            DARK_GRAY   = 0x4208, // Темно-сірий
            GRAY        = 0x8410, // Класичний сірий (50%)
            LIGHT_GRAY  = 0xC618, // Світло-сірий
            SILVER      = 0xCE79, // Сріблястий

            // Додаткові корисні відтінки (помаранчевий, рожевий, золотий тощо)
            ORANGE      = 0xFD20,
            GOLD        = 0xFEA0,
            YELLOW_GREEN= 0x9E66,
            PINK        = 0xF819,
            VIOLET      = 0x919C,
            BROWN       = 0xA145

        } DisplayDriverColor;

        typedef enum : uint16_t
        {
            MADCTL_MY = 0x80,
            MADCTL_MX = 0x40,
            MADCTL_MV = 0x20,
            MADCTL_RGB = 0x00,
            MADCTL_BGR = 0x08,

            NOP = 0x00,
            SWRESET = 0x01,
            RDDID = 0x04,
            RDDST = 0x09,

            SLPIN = 0x10,
            SLPOUT = 0x11,
            PTLON = 0x12,
            NORON = 0x13,

            INVOFF = 0x20,
            INVON = 0x21,
            DISPOFF = 0x28,
            DISPON = 0x29,
            CASET = 0x2A,
            RASET = 0x2B,
            RAMWR = 0x2C,
            RAMRD = 0x2E,

            PTLAR = 0x30,
            COLMOD = 0x3A,
            MADCTL = 0x36,

            FRMCTR1 = 0xB1,
            FRMCTR2 = 0xB2,
            FRMCTR3 = 0xB3,
            INVCTR = 0xB4,
            DISSET5 = 0xB6,

            PWCTR1 = 0xC0,
            PWCTR2 = 0xC1,
            PWCTR3 = 0xC2,
            PWCTR4 = 0xC3,
            PWCTR5 = 0xC4,
            VMCTR1 = 0xC5,

            RDID1 = 0xDA,
            RDID2 = 0xDB,
            RDID3 = 0xDC,
            RDID4 = 0xDD,
            PWCTR6 = 0xFC,
            GMCTRP1 = 0xE0,
            GMCTRN1 = 0xE1,
            DELAY = 0x80,

        } DisplayDriverCmd;
    /*
        Portrait (0)

            TOP
            +---------+
            |         |
            |         |
            |         |
            +---------+
            BOT

        Landscape (1)

            +-------------+
            |             |
        TOP |             | BOT
            |             |
            +-------------+

        PortraitInverted (2)

            BOT
            +---------+
            |         |
            |         |
            |         |
            +---------+
            TOP

        LandscapeInverted (3)

            +-------------+
            |             |
        BOT |             | TOP
            |             |
            +-------------+
    */
        typedef enum : uint16_t
        {
            Portrait = 0,          // 160x128, 
            Landscape = 1,         // 128x160,
            PortraitInverted = 2,  // 160x128, 180°
            LandscapeInverted = 3  // 128x160, 180°

        } DisplayDriverRotation;

        typedef enum : uint16_t
        {
            WIDTH = 128,
            HEIGHT = 160,
            XSTART = 0,
            YSTART = 0,
            DEFAULT_ROTATION = Portrait,
            DATA_ROTATION = (DisplayDriverCmd::MADCTL_MX | DisplayDriverCmd::MADCTL_MY),

        } DisplayDriverConstant;

    private:
        typedef struct
        {
            SPI_HandleTypeDef *hspi;
            DriverGpio gpio;
            int16_t height;
            int16_t width;
            uint8_t xstart;
            uint8_t ystart;
            uint8_t rotationValue;
            uint8_t rotationData[4];
        } DriverConfig;

        void displayReset();
        void csSet(bool state);
        void dcSet(bool state);
        void blSet(bool state);
        bool executeCmdsList(const uint8_t *addr);
        bool writeCmd(const uint8_t cmd);
        bool writeData(const uint8_t *const buff, const size_t buff_size);
        bool setAddressWindow(const uint8_t x0, const uint8_t y0, const uint8_t x1, const uint8_t y1);
        bool setImage(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h, const uint16_t *data);
        bool writeChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);
        bool drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);

        DriverConfig m_config;
        SPI_HandleTypeDef *m_hspi;
        DriverGpio &m_gpios;
    };
}
