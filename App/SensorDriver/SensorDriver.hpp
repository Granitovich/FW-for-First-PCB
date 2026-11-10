
#pragma once
#include "main.h"
#include "stm32g0xx_hal_i2c.h"
#include <cstdint>


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
    class SensorDriver
    {
    public:
        enum class SensorStatus
        {
            /* The value was read, the CRC matches, and data is valid. */
            OK,

            /* The value was read, the CRC matches, but the data is invalid (e.g. the measurement was not yet finished) */
            InvalidData,

            /* The value was read, but the CRC over the payload (valid and data) does not match. */
            InvalidCRC,
        
            /* There was an I2C communication error, `read`ing the value*/
            I2CError,

            InvalidParameter,
        
        };

        explicit SensorDriver(I2C_HandleTypeDef *i2c) : m_i2c (i2c) {};

        typedef enum : uint32_t
        {
            /* Slave address of ENS210 */
            ENS210_ADDRESS = 0x43,

            /* Slave address of ENS210 */
            ENS210_ADDRESS_HAL =  (0x43 << 1),

            /* The expected part id of the ENS210 */
            ENS210_PARTID = 0x0210, 

            /* Booting time in ms (also after reset, or going to high power) */
            ENS210_BOOTING = 2,

            /* Conversion time in ms for single shot T/H measurement */
            ENS210_THCONV_SINGLE_MS = 130,

            /* Conversion time in ms for continuous T/H measurement */
            ENS210_THCONV_CONT_MS = 238,

            /* A 7 bits CRC has polynomial of 7th order, which has 8 terms */
            CRC7WIDTH = 7,

            /* The 8 coefficients of the polynomial */
            CRC7POLY = 0x89,

            /* Initial vector has all 7 bits high */
            CRC7IVEC = 0x7F,

            /* Payload data */
            DATA7WIDTH = 17,

            /* 0b 0 1111 1111 1111 1111*/
            DATA7MASK = ((1UL<<DATA7WIDTH)-1),

            /* 0b 1 0000 0000 0000 0000 */
            DATA7MSB = (1UL<<(DATA7WIDTH-1)),

            ENS210_TRIALS  = 5,
            ENS210_TIMEOUT = 100,
            
            
        } SensorDriverConstant;

        typedef enum : uint16_t
        {
            ENS210_REG_PART_ID    = 0x00,
            ENS210_REG_DIE_REV    = 0x02,
            ENS210_REG_UID        = 0x04,
            ENS210_REG_SYS_CTRL   = 0x10,
            ENS210_REG_SYS_STAT   = 0x11,
            ENS210_REG_SENS_RUN   = 0x21,
            ENS210_REG_SENS_START = 0x22,
            ENS210_REG_SENS_STOP  = 0x23,
            ENS210_REG_SENS_STAT  = 0x24,
            ENS210_REG_T_VAL      = 0x30,
            ENS210_REG_H_VAL      = 0x33
        } SensorDriverRegister;

        
        typedef enum : uint8_t
        {
            SingleShot = 0,
            Continuos  = 1,
 
        } SensorMeasurementMode;

        typedef struct 
        {   
            /* Part ID of ENS210, should be 0x210 */
            uint16_t partID;

            /*  Revision number of the ENS210 device */
            uint16_t revisionNumber;

            /* Unique ID of this specific ENS210*/
            uint64_t uniqueId;

            /* First 32bit of unique ID of this specific ENS210 */
            uint32_t uniqueIdH;
            
            /* Second 32bit of unique ID of this specific ENS210*/
            uint32_t uniqueIdL;

            /* Measurement mode */
            SensorMeasurementMode mode;

            /* Correction due to soldering (in 1/64K); subtracted from `t_data` by conversion functions */
            uint8_t solderCorrection;
            
        } SensorConfig;

        typedef struct 
        {
            uint32_t temperature;
            uint32_t humidity;
        } SensorData;
        
        SensorStatus init();
        SensorStatus setMode(SensorMeasurementMode mode);
        SensorStatus run();
        SensorData getData() { return m_data; }

    private:
        I2C_HandleTypeDef *m_i2c;
        bool writeRegister8(const uint8_t addr, const uint8_t regName, const uint8_t value);
        bool readRegister8 (const uint8_t addr, const uint8_t regName, uint8_t *value);

        bool read (const uint8_t addr, const uint8_t reg, uint8_t *buf, const uint8_t num);
        bool write(const uint8_t addr, const uint8_t reg, uint8_t *buf, const uint8_t num);

        bool getVersion();
        bool setLowPower(bool status);
        bool reset();
        SensorData   m_data;
        SensorConfig m_config;
    };
}





































