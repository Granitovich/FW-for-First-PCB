#include "DisplayManager.hpp"

bool Application::DisplayManager::createInit()
{

    driver.init();
    driver.setRotation(DriverLayer::DisplayDriver::DisplayDriverRotation::Portrait);
    // display.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

    // display.drawImage(Images::MemoryCardErrorPortrait);

    // HAL_Delay(5000);
    driver.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.drawString(20, 10, "MemoryHub", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.drawString(10, 40, "Created by", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.drawString(5, 60, "Granitovich", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.drawString(20, 80, "With Love", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::RED, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.drawString(20, 100, "to Family", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::RED, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    driver.drawString(5, 130, "Version 1.0", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    HAL_Delay(1000);
    driver.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
    // display.drawLine(0,0, 10, 0, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN);
    // display.drawFastHLine(10, 0, 100, DriverLayer::DisplayDriver::DisplayDriverColor::RED);
    // display.drawFastVLine(10, 0, 100, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN);
    currentMenuState = static_cast<DisplayMainMenuState>(DisplayMainMenuState::MainMenu_InitialState);
    createMenu();

    return true;
}
bool Application::DisplayManager::run()
{
    m_btnManager.run();
    mainMenuHadle(m_btnManager.GetChangedButtonState());

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

        if(previousbuttonState == ButtonManager::ButtonState::LongPress)
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
    previousbuttonState = buttonState;

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
        driver.drawString(0, 10, "Settings", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   
        currentSettingsState = DisplaySettingsState::AnimationTime;

        break;
    }
    case DisplaySettingsState::AnimationTime:
    {
        // check sensor data and change display
        driver.drawString(110, 100, "Time of animate", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        break;
    }
    default:
    {
        break;
    }
    }

    return true;
}