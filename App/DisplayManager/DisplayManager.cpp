#include "DisplayManager.hpp"
#include <etl/vector.h>
#include <etl/to_string.h>
#include <stdint.h>

bool Application::DisplayManager::createInit()
{

    driver.init();
    driver.setRotation(DriverLayer::DisplayDriver::DisplayDriverRotation::Portrait);

    driver.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

    driver.drawString(20, 10, "MemoryHub", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GOLD, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.drawString(20, 35, "Made with", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::EMERALD, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.drawString(5, 60, "Love to our", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::RED, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.drawString(5, 80, "Best family", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::RED, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.drawString(25, 110, "Version:", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::EMERALD, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

  
    etl::string<16> version_str;
    auto settings = m_storage.getSettings();

// Форматуємо напряму у масив даних etl::string
snprintf(version_str.data(), version_str.capacity(), "%u.%u.%u",
         settings.versionMajor, 
         settings.versionMinor, 
         settings.versionPatch);

// Оновлюємо внутрішню довжину etl::string після стороннього запису через snprintf
version_str.repair();


    
    driver.drawString(40, 130, version_str.c_str(), Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

    HAL_Delay(1000);
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
        }
        createMenu();

    }

    if (buttonState == ButtonManager::ButtonState::LongPress)
    {
        backToMenu = true;

        if(previousChangedButtonState == ButtonManager::ButtonState::LongPress)
        {
            return true;
        }

        switch (currentMenuState)
        {
        case DisplayMainMenuState::Settings:
        {
            currentSettingsState = DisplaySettingsState::Settings_InitialState;
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
    previousChangedButtonState = buttonState;

    return true;
}

bool Application::DisplayManager::settingsHandle(ButtonManager::ButtonState buttonState)
{
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
        /*
        backToMenu = true;

        if(previousChangedButtonState == ButtonManager::ButtonState::LongPress)
        {
            return true;
        }

        switch (currentMenuState)
        {
        case DisplayMainMenuState::Settings:
        {
            currentSettingsState = DisplaySettingsState::Settings_InitialState;
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

        */
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
        driver.drawString(0, 10, "Sensor Data", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawString(0, 40, "Temperature:", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawString(0, 70, "Humidity:", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawString(0, 100, "Pressure: ", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        currentSensorState = DisplaySensorState::Monitoring;

        break;
    }
    case DisplaySensorState::Monitoring:
    {
        // check sensor data and change display
        driver.drawString(110, 100, "20", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

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
        driver.drawString(0, 10, "Animation", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   
        currentAnimationsState = DisplayAnimationsState::Animate;

        break;
    }
    case DisplayAnimationsState::Animate:
    {
        // check sensor data and change display
        driver.drawString(110, 100, "check", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

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
        driver.drawString(53, 25, "10%", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawString(5, 55, "AnimateTime", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,75, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 80, "10s", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawString(10, 110, "SensorTime", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,130, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 135, "20s", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        break;
    }
    case DisplaySettingsState::Brightness:
    {
        driver.drawString(10, 0, "Brightness", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,20, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 25, "10%", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawString(10, 110, "SensorTime", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,130, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 135, "20s", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        break;
    }
    case DisplaySettingsState::AnimationTime:
    {
        driver.drawString(10, 0, "Brightness", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,20, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 25, "10%", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawString(5, 55, "AnimateTime", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,75, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 80, "10s", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        break;
    }
    case DisplaySettingsState::SensTime:
    {
        driver.drawString(5, 55, "AnimateTime", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,75, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 80, "10s", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        driver.drawString(10, 110, "SensorTime", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawRoundRect(50,130, 40, 30, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
        driver.drawString(53, 135, "20s", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        break;
    }
    default:
    {
        break;
    }
    }

    return true;
}