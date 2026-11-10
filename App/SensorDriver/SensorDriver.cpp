#include "SensorDriver.hpp"
#include <cstdint>

/*
    Compute the CRC-7 of 'val' (should only have 17 bits)
    https://en.wikipedia.org/wiki/Cyclic_redundancy_check#Computation
*/    
static uint32_t crc7( uint32_t val ) 
{
	// Setup polynomial
	uint32_t pol= DriverLayer::SensorDriver::CRC7POLY;
	// Align polynomial with data
	pol = pol << (DriverLayer::SensorDriver::DATA7WIDTH - DriverLayer::SensorDriver::CRC7WIDTH - 1);
	// Loop variable (indicates which bit to test, start with highest)
	uint32_t bit = DriverLayer::SensorDriver::DATA7MSB;
	// Make room for CRC value
	val = val << DriverLayer::SensorDriver::CRC7WIDTH;
	bit = bit << DriverLayer::SensorDriver::CRC7WIDTH;
	pol = pol << DriverLayer::SensorDriver::CRC7WIDTH;
	// Insert initial vector
	val |= DriverLayer::SensorDriver::CRC7IVEC;
	// Apply division until all bits done
	while( bit & (DriverLayer::SensorDriver::DATA7MASK << DriverLayer::SensorDriver::CRC7WIDTH) ) {
		if( bit & val ) val ^= pol;
		bit >>= 1;
		pol >>= 1;
	}
	return val;
}


DriverLayer::SensorDriver::SensorStatus DriverLayer::SensorDriver::init()
{
    if(!getVersion())
    {
        return SensorStatus::I2CError;
    }
    return SensorStatus::OK;
}

DriverLayer::SensorDriver::SensorStatus DriverLayer::SensorDriver::setMode(SensorMeasurementMode mode)
{
    uint8_t currentmode = 0xFF;
    /* TODO: Delete*/
    crc7(currentmode);

    switch (mode)
    {

        case DriverLayer::SensorDriver::SensorMeasurementMode::SingleShot:
        {
            currentmode = DriverLayer::SensorDriver::SensorMeasurementMode::SingleShot;
            if(HAL_OK != HAL_I2C_Mem_Write(m_i2c, (uint16_t)ENS210_ADDRESS_HAL, ENS210_REG_SENS_RUN, I2C_MEMADD_SIZE_8BIT, &currentmode, sizeof(mode), ENS210_TIMEOUT))
            {
                return SensorStatus::I2CError;
            }
            m_config.mode = DriverLayer::SensorDriver::SensorMeasurementMode::SingleShot;
            break;
        }
        case DriverLayer::SensorDriver::SensorMeasurementMode::Continuos:
        {
            currentmode = DriverLayer::SensorDriver::SensorMeasurementMode::Continuos;
            if(HAL_OK != HAL_I2C_Mem_Write(m_i2c, (uint16_t)ENS210_ADDRESS_HAL, ENS210_REG_SENS_RUN, I2C_MEMADD_SIZE_8BIT, &currentmode, sizeof(mode), ENS210_TIMEOUT))
            {
                return SensorStatus::I2CError;
            }
            m_config.mode = DriverLayer::SensorDriver::SensorMeasurementMode::Continuos;
            break;
        }
        
        default:
            break;
    }

    return SensorStatus::OK;
}

DriverLayer::SensorDriver::SensorStatus DriverLayer::SensorDriver::run()
{
    uint8_t startMeasurement = 0x00;
    uint8_t triggerMeasurement = 0x03;
    if(HAL_OK != HAL_I2C_Mem_Write(m_i2c, (uint16_t)ENS210_ADDRESS_HAL, ENS210_REG_SENS_RUN, I2C_MEMADD_SIZE_8BIT, &startMeasurement, sizeof(startMeasurement), ENS210_TIMEOUT))
    {
        return SensorStatus::I2CError;
    }

    if(HAL_OK != HAL_I2C_Mem_Write(m_i2c, (uint16_t)ENS210_ADDRESS_HAL, ENS210_REG_SENS_START, I2C_MEMADD_SIZE_8BIT, &triggerMeasurement, sizeof(triggerMeasurement), ENS210_TIMEOUT))
    {
        return SensorStatus::I2CError;
    }
    /* Wait for the measurement to complete */
    HAL_Delay(ENS210_THCONV_SINGLE_MS);

    if(!readData())
    {
        return SensorStatus::I2CError;
    }

    return SensorStatus::OK;
}

bool DriverLayer::SensorDriver::getVersion()
{
    uint8_t i2cbuf[8] = {0};
	
    /* Did the sensor correct soldered? */
    if(HAL_OK != HAL_I2C_IsDeviceReady(m_i2c, (uint16_t)ENS210_ADDRESS_HAL, ENS210_TRIALS, ENS210_TIMEOUT))
    {
        return false;
    }

    /* Must disable low power to read PART_ID or UID */
    setLowPower(false);
    HAL_Delay(100);
    /* Read the PART_ID */
    if(HAL_OK != HAL_I2C_Mem_Read(m_i2c, (uint16_t)ENS210_ADDRESS_HAL, ENS210_REG_PART_ID, I2C_MEMADD_SIZE_8BIT, i2cbuf, sizeof(uint32_t), ENS210_TIMEOUT))
    {
        return false;
    }

    m_config.partID = (uint16_t)(((uint16_t)i2cbuf[1] << 8) | ((uint16_t)i2cbuf[0]));

    if(HAL_OK != HAL_I2C_Mem_Read(m_i2c, (uint16_t)ENS210_ADDRESS_HAL, ENS210_REG_DIE_REV, I2C_MEMADD_SIZE_8BIT, i2cbuf, sizeof(uint16_t), ENS210_TIMEOUT))
    {
        return false;
    }
    
    m_config.revisionNumber = (uint16_t)(((uint16_t)i2cbuf[1] << 8) | ((uint16_t)i2cbuf[0]));

    /* Read the U_ID */
    if(HAL_OK != HAL_I2C_Mem_Read(m_i2c, (uint16_t)ENS210_ADDRESS_HAL, ENS210_REG_UID, I2C_MEMADD_SIZE_8BIT, i2cbuf, sizeof(i2cbuf), ENS210_TIMEOUT))
    {
        return false;
    }

    m_config.uniqueId = (uint64_t)((uint64_t)i2cbuf[7]<<56 | (uint64_t)i2cbuf[6]<<48 | (uint64_t)i2cbuf[5]<<40 | (uint64_t)i2cbuf[4]<<32 | (uint64_t)i2cbuf[3]<<24 | (uint64_t)i2cbuf[2]<<16 | (uint64_t)i2cbuf[1]<<8 | (uint64_t)i2cbuf[0]);

    m_config.uniqueIdH = (uint32_t)(m_config.uniqueId >> 32);
	m_config.uniqueIdL= (uint32_t)(m_config.uniqueId & 0xFFFFFFFF);

    /* Go back to default power mode (low power enabled) */
    setLowPower(true);

    return true;
}

bool DriverLayer::SensorDriver::setLowPower(bool status)
{
    uint8_t setPower = status ? 0x01: 0x00;
    uint8_t readPower = 0xFF;

    if(HAL_OK != HAL_I2C_Mem_Write(m_i2c, (uint16_t)ENS210_ADDRESS_HAL, ENS210_REG_SYS_CTRL, I2C_MEMADD_SIZE_8BIT, &setPower, sizeof(setPower), ENS210_TIMEOUT))
    {
        return false;
    }
    
    if(HAL_OK != HAL_I2C_Mem_Read(m_i2c, (uint16_t)ENS210_ADDRESS_HAL, ENS210_REG_SYS_CTRL, I2C_MEMADD_SIZE_8BIT, &readPower, sizeof(readPower), ENS210_TIMEOUT))
    {
        return false;
    }
    
    if(setPower != readPower)
    {
        return false;
    }

    return true;
}

bool DriverLayer::SensorDriver::reset()
{
    uint8_t reset = 0x80;

    if(HAL_OK != HAL_I2C_Mem_Write(m_i2c, (uint16_t)ENS210_ADDRESS_HAL, ENS210_REG_SYS_CTRL, I2C_MEMADD_SIZE_8BIT, &reset, sizeof(reset), ENS210_TIMEOUT))
    {
        return false;
    }
    
    return true;
}

bool DriverLayer::SensorDriver::readData()
{
    uint8_t i2cbuf[6] = {0};
	uint8_t valid;
	uint32_t crc;
	uint32_t payload;

    if(HAL_OK != HAL_I2C_Mem_Read(m_i2c, (uint16_t)ENS210_ADDRESS_HAL, ENS210_REG_T_VAL, I2C_MEMADD_SIZE_8BIT, i2cbuf, sizeof(i2cbuf), ENS210_TIMEOUT))
    {
        return false;
    }

    uint32_t tempValue= (uint32_t)((uint32_t)i2cbuf[2]<<16 | (uint32_t)i2cbuf[1]<<8 | (uint32_t)i2cbuf[0]);
    m_data.temperature = (tempValue>>0) & 0xffff;

	valid   = (tempValue>>16) & 0x1;
	crc     = (tempValue>>17) & 0x7f;
	payload = (tempValue>>0 ) & 0x1ffff;

    uint32_t calculatedCRC = crc7(payload);
    if(calculatedCRC != crc)
    {
        return false;
    }
    if(!valid)
    {
        return false;
    }

    uint32_t humidityValue= (uint32_t)((uint32_t)i2cbuf[5]<<16 | (uint32_t)i2cbuf[4]<<8 | (uint32_t)i2cbuf[3]);
	m_data.humidity = (humidityValue>>0) & 0xffff;

	valid   = (humidityValue>>16) & 0x1;
	crc     = (humidityValue>>17) & 0x7f;
	payload = (humidityValue>>0 ) & 0x1ffff;
	
    calculatedCRC = crc7(payload);
    if(calculatedCRC != crc)
    {
        return false;
    }
    if(!valid)
    {
        return false;
    }
    convertData();

    return true;
}

void DriverLayer::SensorDriver::convertData()
{
    uint32_t rawTemperature = m_data.temperature;
    uint32_t rawHumidity    = m_data.humidity;

    /* Convert temperature to 0.01 degree Celsius */
    float tempCelsius = (float)((((float)rawTemperature / 64.0f) - ( 27315/ 100.0f) ));

    /* Convert humidity to 0.01 %RH */
    float humPercent = (float)((float)rawHumidity / 512.0f);

    /* Store the converted values back to m_data */
    m_data.temperature = static_cast<uint32_t>(tempCelsius * 100.0f); // Store as 0.01 degree Celsius
    m_data.humidity    = static_cast<uint32_t>(humPercent * 100.0f);    // Store as 0.01 %RH

}