#include "DisplayDriver.hpp"
#include "utility"
#include <cstdlib>
using namespace DriverLayer;

extern bool transferComplete;
uint8_t DisplayDriver::transferDmaBuffer[DisplayDriver::DisplayDriverConstant::DMA_BUFFER_SIZE];
/*
 * GLib_ST7735R_1.8_TFT
 * Copyright (C) 2025 Anatoliy Lizanets, Andrew Kushyk, Andriy Honcharenko, ScarsFun
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

/* Based on Adafruit ST7735 library for Arduino */
static const uint8_t
    firstInitCmds[] = {                                                                                  // Init for 7735R, part 1 (red or green tab)
                       15,                                                                               // 15 commands in list:
                       DisplayDriver::DisplayDriverCmd::SWRESET, DisplayDriver::DisplayDriverCmd::DELAY, //  1: Software reset, 0 args, w/delay
                       150,                                                                              //     150 ms delay
                       DisplayDriver::DisplayDriverCmd::SLPOUT, DisplayDriver::DisplayDriverCmd::DELAY,  //  2: Out of sleep mode, 0 args, w/delay
                       255,                                                                              //     500 ms delay
                       DisplayDriver::DisplayDriverCmd::FRMCTR1, 3,                                      //  3: Frame rate ctrl - normal mode, 3 args:
                       0x01, 0x2C, 0x2D,                                                                 //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
                       DisplayDriver::DisplayDriverCmd::FRMCTR2, 3,                                      //  4: Frame rate control - idle mode, 3 args:
                       0x01, 0x2C, 0x2D,                                                                 //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
                       DisplayDriver::DisplayDriverCmd::FRMCTR3, 6,                                      //  5: Frame rate ctrl - partial mode, 6 args:
                       0x01, 0x2C, 0x2D,                                                                 //     Dot inversion mode
                       0x01, 0x2C, 0x2D,                                                                 //     Line inversion mode
                       DisplayDriver::DisplayDriverCmd::INVCTR, 1,                                       //  6: Display inversion ctrl, 1 arg, no delay:
                       0x07,                                                                             //     No inversion
                       DisplayDriver::DisplayDriverCmd::PWCTR1, 3,                                       //  7: Power control, 3 args, no delay:
                       0xA2,
                       0x02,                                       //     -4.6V
                       0x84,                                       //     AUTO mode
                       DisplayDriver::DisplayDriverCmd::PWCTR2, 1, //  8: Power control, 1 arg, no delay:
                       0xC5,                                       //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
                       DisplayDriver::DisplayDriverCmd::PWCTR3, 2, //  9: Power control, 2 args, no delay:
                       0x0A,                                       //     Opamp current small
                       0x00,                                       //     Boost frequency
                       DisplayDriver::DisplayDriverCmd::PWCTR4, 2, // 10: Power control, 2 args, no delay:
                       0x8A,                                       //     BCLK/2, Opamp current small & Medium low
                       0x2A,
                       DisplayDriver::DisplayDriverCmd::PWCTR5, 2, // 11: Power control, 2 args, no delay:
                       0x8A, 0xEE,
                       DisplayDriver::DisplayDriverCmd::VMCTR1, 1, // 12: Power control, 1 arg, no delay:
                       0x0E,
                       DisplayDriver::DisplayDriverCmd::INVOFF, 0,          // 13: Don't invert display, no args, no delay
                       DisplayDriver::DisplayDriverCmd::MADCTL, 1,          // 14: Memory access control (directions), 1 arg:
                       DisplayDriver::DisplayDriverConstant::DATA_ROTATION, //     row addr/col addr, bottom to top refresh
                       DisplayDriver::DisplayDriverCmd::COLMOD, 1,          // 15: set color mode, 1 arg, no delay:
                       0x05},                                               //     16-bit color

    secondInitCmds[] = {                                           // Init for 7735R, part 2 (1.44" display)
                        2,                                         //  2 commands in list:
                        DisplayDriver::DisplayDriverCmd::CASET, 4, //  1: Column addr set, 4 args, no delay:
                        0x00, 0x00,                                //     XSTART = 0
                        0x00, 0x7F,                                //     XEND = 127
                        DisplayDriver::DisplayDriverCmd::RASET, 4, //  2: Row addr set, 4 args, no delay:
                        0x00, 0x00,                                //     XSTART = 0
                        0x00, 0x7F},                               //     XEND = 127

    thirdInitCmds[] = {                                                                                                                                                                                // Init for 7735R, part 3 (red or green tab)
                       4,                                                                                                                                                                              //  4 commands in list:
                       DisplayDriver::DisplayDriverCmd::GMCTRP1, 16,                                                                                                                                   //  1: Magical unicorn dust, 16 args, no delay:
                       0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10, DisplayDriver::DisplayDriverCmd::GMCTRN1, 16,                                   //  2: Sparkles and rainbows, 16 args, no delay:
                       0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10, DisplayDriver::DisplayDriverCmd::NORON, DisplayDriver::DisplayDriverCmd::DELAY, //  3: Normal display on, no args, w/delay
                       10,                                                                                                                                                                             //     10 ms delay
                       DisplayDriver::DisplayDriverCmd::DISPON, DisplayDriver::DisplayDriverCmd::DELAY,                                                                                                //  4: Main screen turn on, no args w/delay
                       100};                                                                                                                                                                           //     100 ms delay

__attribute__((always_inline)) static inline void GPIO_SetPin(GPIO_TypeDef *port, uint16_t pin, bool level)
{
    if (level)
    {
        HAL_GPIO_WritePin(port,pin,GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(port,pin,GPIO_PIN_RESET);
    }
}

DisplayDriver::DisplayDriver(SPI_HandleTypeDef *hspi, DriverGpio &gpios) : m_hspi(hspi), m_gpios(gpios) {}

DisplayDriver::DisplayStatus DisplayDriver::init()
{
    m_config.hspi = m_hspi;
    if (m_config.hspi == nullptr)
    {
        return DisplayStatus::InvalidParameter;
    }

    m_config.gpio = m_gpios;

    m_config.height = DisplayDriverConstant::HEIGHT;
    m_config.width  = DisplayDriverConstant::WIDTH;
    m_config.xstart = DisplayDriverConstant::XSTART;
    m_config.ystart = DisplayDriverConstant::YSTART;
    m_config.rotationValue = DisplayDriverConstant::DEFAULT_ROTATION;

    m_config.rotationData[0] = DisplayDriverCmd::MADCTL_MX;
    m_config.rotationData[1] = DisplayDriverCmd::MADCTL_MY;
    m_config.rotationData[2] = DisplayDriverCmd::MADCTL_MV;
    m_config.rotationData[3] = DisplayDriverCmd::MADCTL_RGB;

    csSet(false);

    displayReset();

    if (!executeCmdsList(firstInitCmds)  ||
        !executeCmdsList(secondInitCmds) ||
        !executeCmdsList(thirdInitCmds))
    {
        return DisplayStatus::SendError;
    }

    csSet(true);
    setRotation(m_config.rotationValue);
    
    return DisplayStatus::OK;
}
DisplayDriver::DisplayStatus DisplayDriver::backlightOn()
{
    blSet(true);
    return DisplayStatus::OK;
}

DisplayDriver::DisplayStatus DisplayDriver::backlightOff()
{
    blSet(false);
    return DisplayStatus::OK;
}

DisplayDriver::DisplayStatus DisplayDriver::setRotation(const uint8_t rotation)
{
    // TODO: Refactor to enum
    m_config.rotationValue = rotation % 4;

    csSet(false);

    if (!writeCmd(DisplayDriverCmd::MADCTL))
    {
        return DisplayStatus::SendError;
    }

    switch (m_config.rotationValue)
    {
        case DisplayDriverRotation::Portrait:
        {
            uint8_t dataRotation = (m_config.rotationData[0] | m_config.rotationData[1] | m_config.rotationData[3]);
            if (!writeData(&dataRotation, sizeof(dataRotation)))
            {
                return DisplayStatus::SendError;
            }
            m_config.width  = DisplayDriverConstant::WIDTH; 
            m_config.height = DisplayDriverConstant::HEIGHT;
            m_config.xstart = DisplayDriverConstant::XSTART;
            m_config.ystart = DisplayDriverConstant::YSTART;

            break;
        }
        case DisplayDriver::Landscape:
        {
            uint8_t dataRotation = (m_config.rotationData[1] | m_config.rotationData[2] | m_config.rotationData[3]);
            if (!writeData(&dataRotation, sizeof(dataRotation)))
            {
                return DisplayStatus::SendError;
            }
            m_config.width  = DisplayDriverConstant::HEIGHT; 
            m_config.height = DisplayDriverConstant::WIDTH;
            m_config.xstart = DisplayDriverConstant::YSTART;
            m_config.ystart = DisplayDriverConstant::XSTART;

            break;
        }
        case DisplayDriver::PortraitInverted:
        {
            uint8_t dataRotation =  m_config.rotationData[3];
            if (!writeData(&dataRotation, sizeof(dataRotation)))
            {
                return DisplayStatus::SendError;
            }
            m_config.width  = DisplayDriverConstant::WIDTH; 
            m_config.height = DisplayDriverConstant::HEIGHT;
            m_config.xstart = DisplayDriverConstant::YSTART;
            m_config.ystart = DisplayDriverConstant::XSTART;

            break;
        
        }
        case DisplayDriver::LandscapeInverted:
        {
            uint8_t dataRotation = (m_config.rotationData[0] | m_config.rotationData[2] | m_config.rotationData[3]);
            if (!writeData(&dataRotation, sizeof(dataRotation)))
            {
                return DisplayStatus::SendError;
            }
            m_config.width  = DisplayDriverConstant::HEIGHT; 
            m_config.height = DisplayDriverConstant::WIDTH;
            m_config.xstart = DisplayDriverConstant::YSTART;
            m_config.ystart = DisplayDriverConstant::XSTART;

            break;
        }
        default:
        {
            break;
        }
    }

    csSet(true);

    return DisplayStatus::OK;
}

void DisplayDriver::csSet(bool state)
{
    GPIO_SetPin(m_config.gpio.csPort, m_config.gpio.csPin, state);
}

void DisplayDriver::dcSet(bool state)
{
    GPIO_SetPin(m_config.gpio.dataCmdPort, m_config.gpio.dataCmdPin, state);
}
void DisplayDriver::blSet(bool state)
{
    GPIO_SetPin(m_config.gpio.ledPort, m_config.gpio.ledPin, state);
}

void DisplayDriver::displayReset()
{
    // TODO: Try reduce delay
    GPIO_SetPin(m_config.gpio.resetPort, m_config.gpio.resetPin, false);
    HAL_Delay(20);
    GPIO_SetPin(m_config.gpio.resetPort, m_config.gpio.resetPin, true);
}

bool DisplayDriver::writeCmd(const uint8_t cmd)
{
	dcSet(false);

	if ( HAL_OK != HAL_SPI_Transmit_DMA(m_config.hspi, &cmd, sizeof(cmd)) )
	{
		return false;
	}
    while (m_config.hspi->State == HAL_SPI_STATE_BUSY_TX) {};
    //TODO: check without delay
    
    
    return true;


}

bool DisplayDriver::writeData(const uint8_t *const buff, const size_t buff_size)
{
    dcSet(true);
    
    for(size_t i = 0; i < buff_size; i++)
    {
        transferDmaBuffer[i] = buff[i];
    }
    while(!transferComplete);
    
    csSet(true);
    csSet(false);
    if (HAL_OK != HAL_SPI_Transmit_DMA(m_config.hspi, transferDmaBuffer, buff_size))
    {
        return false;
    }
  

    //while (m_config.hspi->State == HAL_SPI_STATE_BUSY_TX) {};

    return true;
}

bool DisplayDriver::executeCmdsList(const uint8_t *addr)
{
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--)
    {
    	uint8_t cmd = *addr++;

    	if ( !writeCmd(cmd))
    	{
    		return false;
    	}

        numArgs = *addr++;
        /* If high bit set, delay follows args */
        ms = numArgs & DisplayDriverCmd::DELAY;
        numArgs &= ~DisplayDriverCmd::DELAY;
        if(numArgs)
        {
            if (!writeData((uint8_t*)addr, numArgs))
            {
            	return false;
            }
            addr += numArgs;
        }
        if(ms)
        {
            ms = *addr++;
            if(ms == 255)
            {
            	ms = 500;
            }
            HAL_Delay(ms);
        }
    }
    return true;

}

bool DisplayDriver::setAddressWindow(const uint8_t x0, const uint8_t y0, const uint8_t x1, const uint8_t y1)
{
    uint8_t xCoordinate  = x0 + m_config.xstart;
    uint8_t x1Coordinate = x1 + m_config.xstart;
    uint8_t y0Coordinate = y0 + m_config.ystart;
    uint8_t y1Coordinate = y1 + m_config.ystart;

    uint8_t data[] = {0, xCoordinate, 0, x1Coordinate};

    /* Column address set */
    if ( !writeCmd(DisplayDriverCmd::CASET) ||
         !writeData(data, sizeof(data))     )
    {
        return false;
    }

    data[1] = y0Coordinate;
    data[3] = y1Coordinate;

    /* Row address set */
    if (!writeCmd(DisplayDriverCmd::RASET) ||
        !writeData(data, sizeof(data))     ||
        !writeCmd(DisplayDriverCmd::RAMWR))
    {
        return false;
    }

    return true;

}

typedef struct
{
    SPI_HandleTypeDef *hspi;
    uint8_t data_rotation[4];
    int16_t height;
    int16_t width;
    uint8_t xstart;
    uint8_t ystart;
    uint8_t value_rotation;
} driver_st7735_config;

// static driver_st7735_config cfg =
// {
// 		.data_rotation = { ST7735_MADCTL_MX, ST7735_MADCTL_MY, ST7735_MADCTL_MV, ST7735_MADCTL_RGB },
// 		.height = ST7735_HEIGHT,
// 		.width  = ST7735_WIDTH,
// 		.xstart = ST7735_XSTART,
// 		.ystart = ST7735_YSTART

// };

DisplayDriver::DisplayStatus DisplayDriver::drawPixel(const uint16_t x, const uint16_t y, const uint16_t color)
{
    
    if ((x >= m_config.width) || (y >= m_config.height))
    {
        return DisplayStatus::InvalidParameter;
    }

    csSet(false);
    uint8_t colorLowByte  = color >> 8;
    uint8_t colorHighByte = color & 0xFF;
    const uint8_t data[] = {colorLowByte, colorHighByte};

    if (!setAddressWindow(x, y, x + 1, y + 1) ||
        (!writeData(data, sizeof(data))))
    {
        return DisplayStatus::SendError;
    }

    csSet(true);

    return DisplayStatus::OK;

}

DisplayDriver::DisplayStatus DisplayDriver::drawImage(const uint16_t* image)
{
    if (image == nullptr)
    {
        return DisplayStatus::InvalidParameter;
    }
    switch(m_config.rotationValue)
    {
        case DisplayDriverRotation::Portrait:
        {
            uint16_t (*ptr_2d)[DisplayDriverConstant::HEIGHT] = (uint16_t(*)[DisplayDriverConstant::HEIGHT])image;
            if(!setImage(0, 0, DisplayDriverConstant::WIDTH, DisplayDriverConstant::HEIGHT, (uint16_t*) ptr_2d))
            {
                return DisplayStatus::SendError;
            }
        }
        break;
        case DisplayDriver::Landscape:
        {
            uint16_t (*ptr_2d)[DisplayDriverConstant::WIDTH] = (uint16_t(*)[DisplayDriverConstant::WIDTH])image;
            if(!setImage(0, 0, DisplayDriverConstant::WIDTH, DisplayDriverConstant::HEIGHT, (uint16_t*) ptr_2d))
            {
                return DisplayStatus::SendError;
            }
        
        }
        break;
        case DisplayDriver::PortraitInverted:
        {
            uint16_t (*ptr_2d)[DisplayDriverConstant::WIDTH] = (uint16_t(*)[DisplayDriverConstant::WIDTH])image;
            if(!setImage(0, 0, DisplayDriverConstant::WIDTH, DisplayDriverConstant::HEIGHT, (uint16_t*) ptr_2d))
            {
                return DisplayStatus::SendError;
            }
        
        }
        break;
        case DisplayDriver::LandscapeInverted:
        {
            uint16_t (*ptr_2d)[DisplayDriverConstant::WIDTH] = (uint16_t(*)[DisplayDriverConstant::WIDTH])image;
            if(!setImage(0, 0, DisplayDriverConstant::WIDTH, DisplayDriverConstant::HEIGHT, (uint16_t*) ptr_2d))
            {
                return DisplayStatus::SendError;
            }
        }
        break;
    }    
    

    
    return DisplayStatus::OK;
}


bool DisplayDriver::setImage(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h, const uint16_t *data)
{
    if (((x >= m_config.width) || (y >= m_config.height)) || ((x + w - 1) >= m_config.width) || ((y + h - 1) >= m_config.height))
    {
        return false;
    }

    csSet(false);

    if (!setAddressWindow(x, y, x + w - 1, y + h - 1) ||
        !writeData((uint8_t*)data, sizeof(uint16_t) * w * h))
    {
        return false;
    }

    //csSet(true);

    return true;
}

DisplayDriver::DisplayStatus DisplayDriver::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, const uint16_t color)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
        std::swap<int16_t>(x0, y0);
        std::swap<int16_t>(x1, y1);
    }

    if (x0 > x1)
    {
        std::swap<int16_t>(x0, x1);
        std::swap<int16_t>(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for (; x0 <= x1; x0++)
    {
        if (steep)
        {
            if (DisplayDriver::DisplayStatus::OK != drawPixel(y0, x0, color))
            {
                return DisplayStatus::SendError;
            }
        }
        else
        {
            if (DisplayDriver::DisplayStatus::OK != drawPixel(x0, y0, color))
            {
                return DisplayStatus::SendError;
            }
        }
        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
    return DisplayDriver::DisplayStatus::OK;
}

DisplayDriver::DisplayStatus DisplayDriver::fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if ( (x >= m_config.width) || (y >= m_config.height) )
    {
    	return DisplayStatus::InvalidParameter;
    }

    if ((x + w - 1) >= m_config.width)
    {
    	w = m_config.width - x;
    }
    if ((y + h - 1) >= m_config.height)
    {
    	 h = m_config.height - y;
    }

    csSet(false);

    if( !setAddressWindow(x, y, x + w - 1, y + h - 1) )
    {
    	return DisplayStatus::SendError;
    }

    uint8_t colorLowByte  = color >> 8;
    uint8_t colorHighByte = color & 0xFF;

    const uint8_t data[2] = { colorLowByte, colorHighByte };

    dcSet(true);

    uint8_t tbuf[w*2];
    for (y = h; y > 0; y--)
    {
        for (int x = w * 2; x >= 0; x -= 2)
        {
            tbuf[x] = data[0];
            tbuf[x + 1] = data[1];
        }

        if ( HAL_OK != HAL_SPI_Transmit_DMA(m_config.hspi, tbuf, sizeof(tbuf)) )
        {
        	return DisplayStatus::SendError;
        }
        else
        {
            /*Try reduce delay*/
        while (m_config.hspi->State == HAL_SPI_STATE_BUSY_TX) {};
        }
    }

    csSet(true);

    return DisplayStatus::OK;
}

DisplayDriver::DisplayStatus DisplayDriver::fillScreen(uint16_t color)
{
    if(DisplayStatus::OK != fillRectangle(0, 0, m_config.width, m_config.height, color))
    {
        return DisplayStatus::SendError;
    }
    return DisplayStatus::OK;
}

DisplayDriver::DisplayStatus DisplayDriver::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    if ((x >= m_config.width) || (y >= m_config.height))
    {
        return DisplayStatus::InvalidParameter;
    }
    if ((y + h - 1) >= m_config.height)
    {
        h = m_config.height - y;
    }

    if (DisplayStatus::OK != fillRectangle(x, y, 1, h, color))
    {
        return DisplayStatus::SendError;
    }

    return DisplayStatus::OK;
}

DisplayDriver::DisplayStatus DisplayDriver::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    if ((x >= m_config.width) || (y >= m_config.height))
    {
        return DisplayStatus::InvalidParameter;
    }
    if ((x + w - 1) >= m_config.width)
    {
        w = m_config.width - x;
    }

    if (DisplayStatus::OK != fillRectangle(x, y, w, 1, color))
    {
        return DisplayStatus::SendError;
    }

    return DisplayStatus::OK;
}

// DisplayDriver::DisplayStatus DisplayDriver::invertColors(bool invert)
// {
//     csSet(false);
//     if (!writeCmd(invert ? DisplayDriverCmd::INVON : DisplayDriverCmd::INVOFF))
//     {
//         return DisplayStatus::SendError;
//     }
//     csSet(true);

//     return DisplayStatus::OK;
// }

// DisplayDriver::DisplayStatus DisplayDriver::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
// {
//     int16_t f = 1 - r;
//     int16_t ddF_x = 1;
//     int16_t ddF_y = -r - r;
//     int16_t x = 0;

//     if (DisplayStatus::OK != drawPixel(x0 + r, y0, color) ||
//         DisplayStatus::OK != drawPixel(x0 - r, y0, color) ||
//         DisplayStatus::OK != drawPixel(x0, y0 - r, color) ||
//         DisplayStatus::OK != drawPixel(x0, y0 + r, color))
//     {
//         return DisplayStatus::SendError;
//     }

//     while (x < r)
//     {
//         if (f >= 0)
//         {
//             r--;
//             ddF_y += 2;
//             f += ddF_y;
//         }
//         x++;
//         ddF_x += 2;
//         f += ddF_x;

//         if (DisplayStatus::OK != drawPixel(x0 + x, y0 + r, color) ||
//             DisplayStatus::OK != drawPixel(x0 - x, y0 + r, color) ||
//             DisplayStatus::OK != drawPixel(x0 - x, y0 - r, color) ||
//             DisplayStatus::OK != drawPixel(x0 + x, y0 - r, color) ||
//             DisplayStatus::OK != drawPixel(x0 + r, y0 + x, color) ||
//             DisplayStatus::OK != drawPixel(x0 - r, y0 + x, color) ||
//             DisplayStatus::OK != drawPixel(x0 - r, y0 - x, color) ||
//             DisplayStatus::OK != drawPixel(x0 + r, y0 - x, color))
//         {
//             return DisplayStatus::SendError;
//         }
//     }

//     return DisplayStatus::OK;
// }

// DisplayDriver::DisplayStatus DisplayDriver::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
// {
//     int16_t f = 1 - r;
//     int16_t ddF_x = 1;
//     int16_t ddF_y = -2 * r;
//     int16_t x = 0;

//     while (x < r)
//     {
//         if (f >= 0)
//         {
//             r--;
//             ddF_y += 2;
//             f += ddF_y;
//         }
//         x++;
//         ddF_x += 2;
//         f += ddF_x;
//         if (cornername & 0x8)
//         {
//             if (DisplayStatus::OK != drawPixel(x0 - r, y0 + x, color) ||
//                 DisplayStatus::OK != drawPixel(x0 - x, y0 + r, color))
//             {
//                 return DisplayStatus::SendError;
//             }
//         }
//         if (cornername & 0x4)
//         {
//             if (DisplayStatus::OK != drawPixel(x0 + x, y0 + r, color) ||
//                 DisplayStatus::OK != drawPixel(x0 + r, y0 + x, color))
//             {
//                 return DisplayStatus::SendError;
//             }
//         }
//         if (cornername & 0x2)
//         {
//             if (DisplayStatus::OK != drawPixel(x0 + r, y0 - x, color) ||
//                 DisplayStatus::OK != drawPixel(x0 + x, y0 - r, color))
//             {
//                 return DisplayStatus::SendError;
//             }
//         }
//         if (cornername & 0x1)
//         {
//             if (DisplayStatus::OK != drawPixel(x0 - x, y0 - r, color) ||
//                 DisplayStatus::OK != drawPixel(x0 - r, y0 - x, color))
//             {
//                 return DisplayStatus::SendError;
//             }
//         }
//     }

//     return DisplayStatus::OK;
// }

// DisplayDriver::DisplayStatus DisplayDriver::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
// {
//     if (DisplayStatus::OK != drawFastVLine(x0, y0 - r, r + r + 1, color))
//     {
//         return DisplayStatus::SendError;
//     }
//     if (DisplayStatus::OK != fillCircleHelper(x0, y0, r, 3, 0, color))
//     {
//         return DisplayStatus::SendError;
//     }

//     return DisplayStatus::OK;
// }

// DisplayDriver::DisplayStatus DisplayDriver::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
// {
//     int16_t f = 1 - r;
//     int16_t ddF_x = 1;
//     int16_t ddF_y = -r - r;
//     int16_t x = 0;

//     while (x < r)
//     {
//         if (f >= 0)
//         {
//             r--;
//             ddF_y += 2;
//             f += ddF_y;
//         }
//         x++;
//         ddF_x += 2;
//         f += ddF_x;

//         if (cornername & 0x1)
//         {
//             if (DisplayStatus::OK != drawFastVLine(x0 + x, y0 - r, r + r + 1 + delta, color))
//             {
//                 return DisplayStatus::SendError;
//             }
//             if (DisplayStatus::OK != drawFastVLine(x0 + r, y0 - x, x + x + 1 + delta, color))
//             {
//                 return DisplayStatus::SendError;
//             }
//         }
//         if (cornername & 0x2)
//         {
//             if (DisplayStatus::OK != drawFastVLine(x0 - x, y0 - r, r + r + 1 + delta, color))
//             {
//                 return DisplayStatus::SendError;
//             }
//             if (DisplayStatus::OK != drawFastVLine(x0 - r, y0 - x, x + x + 1 + delta, color))
//             {
//                 return DisplayStatus::SendError;
//             }
//         }
//     }

//     return DisplayStatus::OK;
// }

// DisplayDriver::DisplayStatus DisplayDriver::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
// {
//     if (DisplayStatus::OK != drawFastHLine(x + r, y, w - r - r, color) ||
//         DisplayStatus::OK != drawFastHLine(x + r, y + h - 1, w - r - r, color) ||
//         DisplayStatus::OK != drawFastVLine(x, y + r, h - r - r, color) ||
//         DisplayStatus::OK != drawFastVLine(x + w - 1, y + r, h - r - r, color) ||
//         DisplayStatus::OK != drawCircleHelper(x + r, y + r, r, 1, color) ||
//         DisplayStatus::OK != drawCircleHelper(x + w - r - 1, y + r, r, 2, color) ||
//         DisplayStatus::OK != drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color) ||
//         DisplayStatus::OK != drawCircleHelper(x + r, y + h - r - 1, r, 8, color))
//     {
//         return DisplayStatus::SendError;
//     }

//     return DisplayStatus::OK;
// }
// DisplayDriver::DisplayStatus DisplayDriver::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
// {
//     if (DisplayStatus::OK != fillRectangle(x + r, y, w - r - r, h, color) ||
//         DisplayStatus::OK != fillCircleHelper(x + w - r - 1, y + r, r, 1, h - r - r - 1, color) ||
//         DisplayStatus::OK != fillCircleHelper(x + r, y + r, r, 2, h - r - r - 1, color))
//     {
//         return DisplayStatus::SendError;
//     }

//     return DisplayStatus::OK;
// }

// DisplayDriver::DisplayStatus DisplayDriver::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
// {
//     if (DisplayStatus::OK != drawLine(x0, y0, x1, y1, color) ||
//         DisplayStatus::OK != drawLine(x1, y1, x2, y2, color) ||
//         DisplayStatus::OK != drawLine(x2, y2, x0, y0, color))
//     {
//         return DisplayStatus::SendError;
//     }

//     return DisplayStatus::OK;
// }

DisplayDriver::DisplayStatus DisplayDriver::drawString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor)
{
    if (str == nullptr)
    {
        return DisplayStatus::InvalidParameter;
    }

	csSet(false);

    while(*str)
    {
        if(x + font.width >= m_config.width)
        {
            x = 0;
            y += font.height;
            if(y + font.height >= m_config.height)
            {
                break;
            }

            if(*str == ' ')
            {
                /* skip spaces in the beginning of the new line */
                str++;
                continue;
            }
        }

        if (!writeChar(x, y, *str, font, color, bgcolor))
        {
            return DisplayStatus::SendError;
        }
        x += font.width;
        str++;
    }
   
    csSet(true);

    return DisplayStatus::OK;

}

bool DisplayDriver::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;

    while (x < r)
    {
        if (f >= 0)
        {
            r--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x8)
        {
            if (DisplayDriver::DisplayStatus::OK != drawPixel(x0 - r, y0 + x, color))
            {
                return false;
            }
            if (DisplayDriver::DisplayStatus::OK != drawPixel(x0 - x, y0 + r, color))
            {
                return false;
            }
        }
        if (cornername & 0x4)
        {
            if (DisplayDriver::DisplayStatus::OK != drawPixel(x0 + x, y0 + r, color))
            {
                return false;
            }
            if (DisplayDriver::DisplayStatus::OK != drawPixel(x0 + r, y0 + x, color))
            {
                return false;
            }
        }
        if (cornername & 0x2)
        {
            if (DisplayDriver::DisplayStatus::OK != drawPixel(x0 + r, y0 - x, color))
            {
                return false;
            }
            if (DisplayDriver::DisplayStatus::OK != drawPixel(x0 + x, y0 - r, color))
            {
                return false;
            }
        }
        if (cornername & 0x1)
        {
            if (DisplayDriver::DisplayStatus::OK != drawPixel(x0 - x, y0 - r, color))
            {
                return false;
            }
            if (DisplayDriver::DisplayStatus::OK != drawPixel(x0 - r, y0 - x, color))
            {
                return false;
            }
        }
    }

    return true;
}

DisplayDriver::DisplayStatus DisplayDriver::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    
    // smarter version
    if(DisplayDriver::DisplayStatus::OK != drawFastHLine(x + r, y, w - r - r, color))         // Top
    {
        return DisplayDriver::DisplayStatus::SendError;
    }
    if(DisplayDriver::DisplayStatus::OK != drawFastHLine(x + r, y, w - r - r, color))         // Top
    {
        return DisplayDriver::DisplayStatus::SendError;
    }
    if(DisplayDriver::DisplayStatus::OK != drawFastHLine(x + r, y + h - 1, w - r - r, color)) // Bottom
    {
        return DisplayDriver::DisplayStatus::SendError;
    }
    if(DisplayDriver::DisplayStatus::OK != drawFastVLine(x, y + r, h - r - r, color))         // Left
    {
        return DisplayDriver::DisplayStatus::SendError;
    }
    if(DisplayDriver::DisplayStatus::OK != drawFastVLine(x + w - 1, y + r, h - r - r, color)) // Right
    {
        return DisplayDriver::DisplayStatus::SendError;
    }
    // draw four corners
    if(!drawCircleHelper(x + r, y + r, r, 1, color))
    {
        return DisplayDriver::DisplayStatus::SendError;
    }
    if(!drawCircleHelper(x + r, y + h - r - 1, r, 8, color))
    {
        return DisplayDriver::DisplayStatus::SendError;
    }
    if(!drawCircleHelper(x + w - r - 1, y + r, r, 2, color))
    {
        return DisplayDriver::DisplayStatus::SendError;
    }
    if(!drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color))
    {
        return DisplayDriver::DisplayStatus::SendError;
    }

    return DisplayDriver::DisplayStatus::OK;

}
// DisplayStatus fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
// {

// }

bool DisplayDriver::writeChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
    uint32_t i, b, j;

    if (!setAddressWindow(x, y, x + font.width - 1, y + font.height - 1))
    {
        return false;
    }

    for (i = 0; i < font.height; i++)
    {
        b = font.data[(ch - 32) * font.height + i];
        for (j = 0; j < font.width; j++)
        {
            if ((b << j) & 0x8000)
            {
                uint8_t colorLowByte  = color >> 8;
                uint8_t colorHighByte = color & 0xFF;

                const uint8_t data[2] = { colorLowByte, colorHighByte };

                    if (!writeData(data, sizeof(data)))
                    {
                        return false;
                    }
            }
            else
            {
                uint8_t colorLowByte  = bgcolor >> 8;
                uint8_t colorHighByte = bgcolor & 0xFF;
                const uint8_t data[2] = { colorLowByte, colorHighByte };
                if (!writeData(data, sizeof(data)))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

#ifdef OTHER_FUNCTIONALITY

static void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);

void ST7735_DrawString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor)
{
    GPIO_SetPin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, false);

    while (*str)
    {
        if (x + font.width >= cfg.width)
        {
            x = 0;
            y += font.height;
            if (y + font.height >= cfg.height)
            {
                break;
            }

            if (*str == ' ')
            {
                /* skip spaces in the beginning of the new line */
                str++;
                continue;
            }
        }

        ST7735_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }
    GPIO_SetPin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, true);
}

void ST7735_FillScreen(uint16_t color)
{
    ST7735_FillRectangle(0, 0, cfg.width, cfg.height, color);
}

void ST7735_InvertColors(bool invert)
{
    GPIO_SetPin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, false);
    ST7735_WriteCommand(invert ? ST7735_INVON : ST7735_INVOFF);
    GPIO_SetPin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, true);
}

/***************************************************************************************
** Function name:           drawCircle
** Description:             Draw a circle outline
***************************************************************************************/
void ST7735_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -r - r;
    int16_t x = 0;

    ST7735_DrawPixel(x0 + r, y0, color);
    ST7735_DrawPixel(x0 - r, y0, color);
    ST7735_DrawPixel(x0, y0 - r, color);
    ST7735_DrawPixel(x0, y0 + r, color);

    while (x < r)
    {
        if (f >= 0)
        {
            r--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ST7735_DrawPixel(x0 + x, y0 + r, color);
        ST7735_DrawPixel(x0 - x, y0 + r, color);
        ST7735_DrawPixel(x0 - x, y0 - r, color);
        ST7735_DrawPixel(x0 + x, y0 - r, color);

        ST7735_DrawPixel(x0 + r, y0 + x, color);
        ST7735_DrawPixel(x0 - r, y0 + x, color);
        ST7735_DrawPixel(x0 - r, y0 - x, color);
        ST7735_DrawPixel(x0 + r, y0 - x, color);
    }
}

/***************************************************************************************
** Function name:           drawCircleHelper
** Description:             Support function for circle drawing
***************************************************************************************/
void ST7735_DrawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;

    while (x < r)
    {
        if (f >= 0)
        {
            r--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x8)
        {
            ST7735_DrawPixel(x0 - r, y0 + x, color);
            ST7735_DrawPixel(x0 - x, y0 + r, color);
        }
        if (cornername & 0x4)
        {
            ST7735_DrawPixel(x0 + x, y0 + r, color);
            ST7735_DrawPixel(x0 + r, y0 + x, color);
        }
        if (cornername & 0x2)
        {
            ST7735_DrawPixel(x0 + r, y0 - x, color);
            ST7735_DrawPixel(x0 + x, y0 - r, color);
        }
        if (cornername & 0x1)
        {
            ST7735_DrawPixel(x0 - x, y0 - r, color);
            ST7735_DrawPixel(x0 - r, y0 - x, color);
        }
    }
}

/***************************************************************************************
** Function name:           fillCircle
** Description:             draw a filled circle
***************************************************************************************/
void ST7735_FillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    ST7735_DrawFastVLine(x0, y0 - r, r + r + 1, color);
    ST7735_FillCircleHelper(x0, y0, r, 3, 0, color);
}

/***************************************************************************************
** Function name:           fillCircleHelper
** Description:             Support function for filled circle drawing
***************************************************************************************/
// Used to do circles and roundrects
void ST7735_FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -r - r;
    int16_t x = 0;

    delta++;
    while (x < r)
    {
        if (f >= 0)
        {
            r--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (cornername & 0x1)
        {
            ST7735_DrawFastVLine(x0 + x, y0 - r, r + r + delta, color);
            ST7735_DrawFastVLine(x0 + r, y0 - x, x + x + delta, color);
        }
        if (cornername & 0x2)
        {
            ST7735_DrawFastVLine(x0 - x, y0 - r, r + r + delta, color);
            ST7735_DrawFastVLine(x0 - r, y0 - x, x + x + delta, color);
        }
    }
}

/***************************************************************************************
** Function name:           drawEllipse
** Description:             Draw a ellipse outline
***************************************************************************************/
void ST7735_DrawEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color)
{
    if (rx < 2)
        return;
    if (ry < 2)
        return;
    int16_t x, y;
    int32_t rx2 = rx * rx;
    int32_t ry2 = ry * ry;
    int32_t fx2 = 4 * rx2;
    int32_t fy2 = 4 * ry2;
    int32_t s;

    for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++)
    {
        ST7735_DrawPixel(x0 + x, y0 + y, color);
        ST7735_DrawPixel(x0 - x, y0 + y, color);
        ST7735_DrawPixel(x0 - x, y0 - y, color);
        ST7735_DrawPixel(x0 + x, y0 - y, color);
        if (s >= 0)
        {
            s += fx2 * (1 - y);
            y--;
        }
        s += ry2 * ((4 * x) + 6);
    }

    for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++)
    {
        ST7735_DrawPixel(x0 + x, y0 + y, color);
        ST7735_DrawPixel(x0 - x, y0 + y, color);
        ST7735_DrawPixel(x0 - x, y0 - y, color);
        ST7735_DrawPixel(x0 + x, y0 - y, color);
        if (s >= 0)
        {
            s += fy2 * (1 - x);
            x--;
        }
        s += rx2 * ((4 * y) + 6);
    }
}

/***************************************************************************************
** Function name:           fillEllipse
** Description:             draw a filled ellipse
***************************************************************************************/
void ST7735_FillEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color)
{
    if (rx < 2)
        return;
    if (ry < 2)
        return;
    int16_t x, y;
    int32_t rx2 = rx * rx;
    int32_t ry2 = ry * ry;
    int32_t fx2 = 4 * rx2;
    int32_t fy2 = 4 * ry2;
    int32_t s;

    for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++)
    {
        ST7735_DrawFastHLine(x0 - x, y0 - y, x + x + 1, color);
        ST7735_DrawFastHLine(x0 - x, y0 + y, x + x + 1, color);

        if (s >= 0)
        {
            s += fx2 * (1 - y);
            y--;
        }
        s += ry2 * ((4 * x) + 6);
    }

    for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++)
    {
        ST7735_DrawFastHLine(x0 - x, y0 - y, x + x + 1, color);
        ST7735_DrawFastHLine(x0 - x, y0 + y, x + x + 1, color);

        if (s >= 0)
        {
            s += fy2 * (1 - x);
            x--;
        }
        s += rx2 * ((4 * y) + 6);
    }
}

/***************************************************************************************
** Function name:           drawRect
** Description:             Draw a rectangle outline
***************************************************************************************/
// Draw a rectangle
void ST7735_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    ST7735_DrawFastHLine(x, y, w, color);
    ST7735_DrawFastHLine(x, y + h - 1, w, color);
    ST7735_DrawFastVLine(x, y, h, color);
    ST7735_DrawFastVLine(x + w - 1, y, h, color);
}

/***************************************************************************************
** Function name:           drawRoundRect
** Description:             Draw a rounded corner rectangle outline
***************************************************************************************/
// Draw a rounded rectangle
void ST7735_DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    // smarter version
    ST7735_DrawFastHLine(x + r, y, w - r - r, color);         // Top
    ST7735_DrawFastHLine(x + r, y + h - 1, w - r - r, color); // Bottom
    ST7735_DrawFastVLine(x, y + r, h - r - r, color);         // Left
    ST7735_DrawFastVLine(x + w - 1, y + r, h - r - r, color); // Right
    // draw four corners
    ST7735_DrawCircleHelper(x + r, y + r, r, 1, color);
    ST7735_DrawCircleHelper(x + r, y + h - r - 1, r, 8, color);
    ST7735_DrawCircleHelper(x + w - r - 1, y + r, r, 2, color);
    ST7735_DrawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
}

/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
// Fill a rounded rectangle
void ST7735_FillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    // smarter version
    ST7735_FillRectangle(x + r, y, w - r - r, h, color);

    // draw four corners
    ST7735_FillCircleHelper(x + w - r - 1, y + r, r, 1, h - r - r - 1, color);
    ST7735_FillCircleHelper(x + r, y + r, r, 2, h - r - r - 1, color);
}

/***************************************************************************************
** Function name:           drawTriangle
** Description:             Draw a triangle outline using 3 arbitrary points
***************************************************************************************/
// Draw a triangle
void ST7735_DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    ST7735_DrawLine(x0, y0, x1, y1, color);
    ST7735_DrawLine(x1, y1, x2, y2, color);
    ST7735_DrawLine(x2, y2, x0, y0, color);
}

/***************************************************************************************
** Function name:           fillTriangle
** Description:             Draw a filled triangle using 3 arbitrary points
***************************************************************************************/
// Fill a triangle - original Adafruit function works well and code footprint is small
void ST7735_FillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1)
    {
        swap_int16_t(&y0, &y1);
        swap_int16_t(&x0, &x1);
    }

    if (y1 > y2)
    {
        swap_int16_t(&y2, &y1);
        swap_int16_t(&x2, &x1);
    }

    if (y0 > y1)
    {
        swap_int16_t(&y0, &y1);
        swap_int16_t(&x0, &x1);
    }

    if (y0 == y2)
    { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if (x1 < a)
            a = x1;
        else if (x1 > b)
            b = x1;
        if (x2 < a)
            a = x2;
        else if (x2 > b)
            b = x2;
        ST7735_DrawFastHLine(a, y0, b - a + 1, color);
        return;
    }

    int16_t
        dx01 = x1 - x0,
        dy01 = y1 - y0,
        dx02 = x2 - x0,
        dy02 = y2 - y0,
        dx12 = x2 - x1,
        dy12 = y2 - y1,
        sa = 0,
        sb = 0;

    /*
       For upper part of triangle, find scanline crossings for segments
       0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
       is included here (and second loop will be skipped, avoiding a /0
       error there), otherwise scanline y1 is skipped here and handled
       in the second loop...which also avoids a /0 error here if y0=y1
       (flat-topped triangle).
    */

    if (y1 == y2)
        last = y1; // Include y1 scanline
    else
        last = y1 - 1; // Skip it

    for (y = y0; y <= last; y++)
    {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;

        if (a > b)
            swap_int16_t(&a, &b);
        ST7735_DrawFastHLine(a, y, b - a + 1, color);
    }
    /*
       For lower part of triangle, find scanline crossings for segments
       0-2 and 1-2.  This loop is skipped if y1=y2.
    */
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for (; y <= y2; y++)
    {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;

        if (a > b)
            swap_int16_t(&a, &b);
        ST7735_DrawFastHLine(a, y, b - a + 1, color);
    }
}

uint8_t ST7735_GetRotation(void)
{
    return cfg.value_rotation;
}

int16_t ST7735_GetHeight(void)
{
    return cfg.height;
}

int16_t ST7735_GetWidth(void)
{
    return cfg.width;
}

static void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
    uint32_t i, b, j;

    ST7735_SetAddressWindow(x, y, x + font.width - 1, y + font.height - 1);

    for (i = 0; i < font.height; i++)
    {
        b = font.data[(ch - 32) * font.height + i];
        for (j = 0; j < font.width; j++)
        {
            if ((b << j) & 0x8000)
            {
                uint8_t data[] = {color >> 8, color & 0xFF};
                ST7735_WriteData(data, sizeof(data));
            }
            else
            {
                uint8_t data[] = {bgcolor >> 8, bgcolor & 0xFF};
                ST7735_WriteData(data, sizeof(data));
            }
        }
    }
}

inline uint16_t ST7735_Color565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}


driver_st7735_status ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if ((x >= cfg.width) || (y >= cfg.height))
    {
        return DRIVER_ST7735_STATUS_INVALID_PARAMETERS;
    }

    if ((x + w - 1) >= cfg.width)
    {
        w = cfg.width - x;
    }
    if ((y + h - 1) >= cfg.height)
    {
        h = cfg.height - y;
    }

    GPIO_SetPin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, false);

    if (DRIVER_ST7735_STATUS_OK != ST7735_SetAddressWindow(x, y, x + w - 1, y + h - 1))
    {
        return DRIVER_ST7735_STATUS_SEND_ERROR;
    }

    const uint8_t data[2] = {color >> 8, color & 0xFF};
    GPIO_SetPin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, true);

    uint8_t tbuf[w * 2];
    for (y = h; y > 0; y--)
    {
        for (int x = w * 2; x >= 0; x -= 2)
        {
            tbuf[x] = data[0];
            tbuf[x + 1] = data[1];
        }
        if (HAL_OK != HAL_SPI_Transmit_DMA(cfg.hspi, tbuf, sizeof(tbuf)))
        {
            return DRIVER_ST7735_STATUS_SEND_ERROR;
        }
        else
        {
            while (cfg.hspi->State == HAL_SPI_STATE_BUSY_TX)
            {
            };
        }
    }

    GPIO_SetPin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, true);

    return DRIVER_ST7735_STATUS_OK;
}

#endif
