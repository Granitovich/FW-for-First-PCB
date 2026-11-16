#include "DisplayManager.hpp"
#include <etl/vector.h>
#include <etl/to_string.h>
#include <stdint.h>
#include <etl/string.h>

bool Application::DisplayManager::createInit()
{
    sensor.init();
    sensor.setMode(DriverLayer::SensorDriver::SensorMeasurementMode::SingleShot);
    sensor.run();

    driver.init();

    if(!m_sdCardManager.init())
    {
        driver.setRotation(DriverLayer::DisplayDriver::DisplayDriverRotation::Portrait);
        driver.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawImage((const uint16_t*)Images::MemoryCardErrorPortrait);
        driver.backlightOn();
        return false;
    }

    driver.setRotation(DriverLayer::DisplayDriver::DisplayDriverRotation::Portrait);

    driver.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.backlightOn();

    driver.drawString(20, 10, "MemoryHub", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GOLD, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.drawString(20, 35, "Made with", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::EMERALD, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.drawString(5, 60, "Love to our", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::RED, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.drawString(5, 80, "Best family", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::RED, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.drawString(25, 110, "Version:", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::EMERALD, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

    driver.drawString(40, 130, m_storage.getVersion().c_str(), Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

    HAL_Delay(2000);
    driver.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

    currentMenuState = static_cast<DisplayMainMenuState>(DisplayMainMenuState::MainMenu_InitialState);
    createMenu();

    return true;
}
bool Application::DisplayManager::run()
{
    m_btnManager.run();
    mainMenuHadle(m_btnManager.GetChangedButtonState());
    settingsHandle(m_btnManager.GetSetButtonState());

    return true;
}

bool Application::DisplayManager::mainMenuHadle(ButtonManager::ButtonState buttonState)
{
    if (buttonState == ButtonManager::ButtonState::ShortPress)
    {
        if (currentMenuIndex < DisplayMainMenuState::StateCount - 1)
        {
            currentMenuIndex++;
        }
        else
        {
            currentMenuIndex = 0;
        }
        currentMenuState = static_cast<DisplayMainMenuState>(currentMenuIndex);

        if (backToMenu)
        {
            currentMenuState = static_cast<DisplayMainMenuState>(DisplayMainMenuState::MainMenu_InitialState);
            currentMenuIndex = 0;
            backToMenu = false;
            if(isAnimationRunning)
            {
                isAnimationRunning = false;
                HAL_Delay(backFromAnimationDelay);
            }
        }
        createMenu();

    }

    if (buttonState == ButtonManager::ButtonState::LongPress)
    {
        backToMenu = true;

        if(previousChangedButtonState == ButtonManager::ButtonState::LongPress)
        {
            previousChangedButtonState = buttonState;
            return true;
        }

        switch (currentMenuState)
        {
        case DisplayMainMenuState::Settings:
        {
            currentSettingsState = DisplaySettingsState::Settings_InitialState;
            currentSettingsIndex = 0;
            createSettings();

            break;
        }

        case DisplayMainMenuState::Animation:
        {
           currentAnimationsState = DisplayAnimationsState::Animation_InitialState;
           createAnimation();

            break;
        }

        case DisplayMainMenuState::Sensoring:
        {
            currentSensorState = DisplaySensorState::Sensor_InitialState;
            createSensor();
            break;
        }

        default:
            break;
        }
        previousMenuState = currentMenuState;
    }
    


    if(currentMenuState == DisplayMainMenuState::Animation && backToMenu && buttonState != ButtonManager::ButtonState::Pressing)
    {
        createAnimation();
    }

    else if(currentMenuState == DisplayMainMenuState::Sensoring && backToMenu && buttonState != ButtonManager::ButtonState::Pressing)
    {
        createSensor();
    }

    previousChangedButtonState = buttonState;

    return true;
}



bool Application::DisplayManager::settingsHandle(ButtonManager::ButtonState buttonState)
{
    if(!backToMenu )
    {
        return true;
    }

    if (buttonState == ButtonManager::ButtonState::ShortPress)
    {
        if (currentSettingsIndex < DisplaySettingsState::Settings_Count - 1)
        {
            currentSettingsIndex++;
        }
        else
        {
            currentSettingsIndex = 0;
        }
        currentSettingsState = static_cast<DisplaySettingsState>(currentSettingsIndex);
        createSettings();

    }

    if (buttonState == ButtonManager::ButtonState::LongPress)
    {
        if(previousSetButtonState == ButtonManager::ButtonState::LongPress)
        {
            previousSetButtonState = buttonState;
            return true;
        }

        currentBrightnessIndex = m_storage.getSettings().brightLevel;
        currentAnimationIndex = m_storage.getSettings().animationTime;
        currentSensorIndex = m_storage.getSettings().sensorTime;

        switch (currentSettingsIndex)
        {
            case DisplaySettingsState::Brightness:
            {
                if (currentBrightnessIndex < m_storage.Brightness_Count)
                {
                    currentBrightnessIndex++;
                }
                else
                {
                    currentBrightnessIndex = 1;
                }

                StorageManager::SettingsData settingsData = {0, 0, 0, 0, 0, 0, 0};
                settingsData.brightLevel = static_cast<uint16_t>(currentBrightnessIndex);

                m_storage.writeSettings(settingsData);

                currentSettingsState = DisplaySettingsState::Brightness;
                createSettings();

                break;
            }
            case DisplaySettingsState::AnimationTime:
            {
                if (currentAnimationIndex < m_storage.AnimationTime_Count)
                {
                    currentAnimationIndex++;
                }
                else
                {
                    currentAnimationIndex = 1;
                }

                StorageManager::SettingsData settingsData = {0, 0, 0, 0, 0, 0, 0};
                settingsData.animationTime = static_cast<uint16_t>(currentAnimationIndex);

                m_storage.writeSettings(settingsData);

                currentSettingsState = DisplaySettingsState::AnimationTime;
                createSettings();

                break;
            }
            case DisplaySettingsState::SensTime:
            {
                if (currentSensorIndex < m_storage.SensorTime_Count)
                {
                    currentSensorIndex++;
                }
                else
                {
                    currentSensorIndex = 1;
                }

                StorageManager::SettingsData settingsData = {0, 0, 0, 0, 0, 0, 0};
                settingsData.sensorTime = static_cast<uint16_t>(currentSensorIndex);

                m_storage.writeSettings(settingsData);

                currentSettingsState = DisplaySettingsState::SensTime;
                createSettings();

                break;
            }
     
            default:
                break;
            }
    
    
    }
    previousSetButtonState = buttonState;
    return true;
}

bool Application::DisplayManager::createMenu()
{
    switch (currentMenuState)
    {
    case DisplayMainMenuState::Settings:
    {
        driver.drawRoundRect(10, 10, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(20, 20, "Settings", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawRoundRect(10, 110, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(15, 120, "Sensoring", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        break;
    }

    case DisplayMainMenuState::Animation:
    {
        driver.drawRoundRect(10, 10, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(20, 20, "Settings", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawRoundRect(10, 60, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(15, 70, "Animation", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        break;
    }

    case DisplayMainMenuState::Sensoring:
    {
        driver.drawRoundRect(10, 110, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(15, 120, "Sensoring", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawRoundRect(10, 60, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(15, 70, "Animation", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        break;
    }

    case DisplayMainMenuState::MainMenu_InitialState:
    {
        driver.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(10, 10, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(20, 20, "Settings", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawRoundRect(10, 60, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(15, 70, "Animation", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawRoundRect(10, 110, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(15, 120, "Sensoring", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        currentMenuState = DisplayMainMenuState::Settings;

        break;
    }

    default:
        break;
    }
    return true;
}

bool Application::DisplayManager::createSensor()
{
    switch (currentSensorState)
    {
    case DisplaySensorState::Sensor_InitialState:
    {
        driver.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawString(5, 10, "Sensor Data", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawString(5, 40, "Temperature", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(20, 70, 100, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(25, 75, "20.00 *C", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawString(25, 110, "Humidity", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(20, 130, 100, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(25, 135, "20.00  %", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
      
        currentSensorState = DisplaySensorState::Monitoring;

        break;
    }
    case DisplaySensorState::Monitoring:
    {
        sensorExecute();
        break;
    }
    default:
    {
        break;
    }
    }

    return true;
}

bool Application::DisplayManager::createAnimation()
{
    switch (currentAnimationsState)
    {
    case DisplayAnimationsState::Animation_InitialState:
    {
        driver.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawString(15, 10, "Animation", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawString(400, 50, "Mode", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        m_sdCardManager.openFile(fileCountName);
        m_sdCardManager.readFile(readWriteBuffer);
        m_sdCardManager.closeFile();

        frameCount = getFrameCount();
        

        driver.drawString(0, 90, "Image found", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawString(0, 110, "In SD card:", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawString(50, 130, getImagesCount().c_str(), Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        m_sdCardManager.openFile(fileImagesName);

        currentAnimationsState = DisplayAnimationsState::Animate;

        break;
    }
    case DisplayAnimationsState::Animate:
    {
        animation();
        isAnimationRunning = true;

        break;
    }
    default:
    {
        break;
    }
    }

    return true;
}

bool Application::DisplayManager::createSettings()
{
    switch (currentSettingsState)
    {
    case DisplaySettingsState::Settings_InitialState:
    {
        driver.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        
        driver.drawString(10, 0, "Brightness", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,20, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);

        driver.drawString(53, 25, m_storage.getBrightness().c_str(), Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawString(5, 55, "AnimateTime", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,75, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 80, m_storage.getAnimationTime().c_str(), Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawString(10, 110, "SensorTime", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,130, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 135, m_storage.getSensorTime().c_str(), Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        break;
    }
    case DisplaySettingsState::Brightness:
    {
        driver.drawString(10, 0, "Brightness", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,20, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 25, m_storage.getBrightness().c_str(), Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawString(10, 110, "SensorTime", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,130, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 135, m_storage.getSensorTime().c_str(), Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        break;
    }
    case DisplaySettingsState::AnimationTime:
    {
        driver.drawString(10, 0, "Brightness", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,20, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 25, m_storage.getBrightness().c_str(), Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawString(5, 55, "AnimateTime", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,75, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 80, m_storage.getAnimationTime().c_str(), Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        break;
    }
    case DisplaySettingsState::SensTime:
    {
        driver.drawString(5, 55, "AnimateTime", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,75, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 80, m_storage.getAnimationTime().c_str(), Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawString(10, 110, "SensorTime", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,130, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 135, m_storage.getSensorTime().c_str(), Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        break;
    }
    default:
    {
        break;
    }
    }

    return true;
}

bool Application::DisplayManager::animation()
{
    static uint32_t animationCounter;
    uint32_t currentTime = HAL_GetTick();

    if(currentTime - imageShowedLastTime <= m_storage.getSettings().animationTime * 1000)
    {
        return true;

    }

    if(animationCounter >= frameCount)
    {
        m_sdCardManager.openFile(fileImagesName);
        animationCounter = 0;
    }
    else
    {
        animationCounter++;
    }
    
    uint32_t offset = animationCounter * frameSize;

    m_sdCardManager.seekFile(fileImagesName, offset);
    m_sdCardManager.readImage(imageBuffer, frameSize);

    driver.drawImage(imageBuffer);

    imageShowedLastTime = currentTime;
    
    return true;
}

bool Application::DisplayManager::sensorExecute()
{
    uint32_t currentTime = HAL_GetTick();

    if(currentTime - sensorUpdatedLastTime <= m_storage.getSettings().sensorTime * 1000)
    {
        if(sensorNeedDataUpdate)
        {
            if(currentTime - sensorUpdatedLastTime >= m_storage.SensorTime_Measurement)
            {
                if(sensor.getStatus() == DriverLayer::SensorDriver::SensorStatus::Ready)
                {
                    sensor.readData();
                    driver.drawString(25, 75,  sensor.getData().temperatureStr.c_str(), Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
                    driver.drawString(25, 135, sensor.getData().humidityStr.c_str(), Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
                    sensorNeedDataUpdate = false;
                    return true;
                }
            }
            
        }

        return true;
    }

    sensor.run();
    sensorNeedDataUpdate = true;
    sensorUpdatedLastTime = currentTime;
    
    return true;
}

uint32_t Application::DisplayManager::getFrameCount()
{
    etl::string<maxCountLength> raw_chars(readWriteBuffer, maxCountLength);

    uint32_t result = 0;

    for (char c : raw_chars) 
    {
        if (c != 'X' && c != 'x') 
        {
            if (c >= '0' && c <= '9') 
            {
                result = (result * 10) + (c - '0');
            }
        }
    }
    
    return result;

}

etl::string<Application::DisplayManager::maxCountLength> Application::DisplayManager::getImagesCount() 
{
  etl::string<maxCountLength> ver;

  char countBuf[maxCountLength];
  snprintf(countBuf, sizeof(countBuf), "%lu", frameCount);
  ver.repair();

  ver = countBuf;
  return ver;
}