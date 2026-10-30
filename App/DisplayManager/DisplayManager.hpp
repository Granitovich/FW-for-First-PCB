#pragma once

#include "DisplayDriver/DisplayDriver.hpp" 
#include "ButtonManager/ButtonManager.hpp"

namespace Application
{
    class DisplayManager 
    {

    public:
        DisplayManager(DriverLayer::DisplayDriver &dispDriver, ButtonManager &btnManager) : driver(dispDriver), m_btnManager(btnManager), currentState(DisplayState::MAIN_MENU) {}
        bool createInit();
        bool createMenu();
        bool createSettings();
        bool createAnimation();
        bool createSensor();
        bool run();
        bool mainMenuHadle(ButtonManager::ButtonState buttonState);

        typedef enum 
        {
            ANIMATION,
            MAIN_MENU,
            SETTINGS,
            SENSOR_DATA
        } DisplayState;

        typedef enum 
        {
            Settings = 0,
            Animation,
            Sensoring,
            StateCount,
            MainMenu_InitialState
        } DisplayMainMenuState;

        typedef enum 
        {
            Brightness = 0,
            AnimationTime,
            Settings_InitialState
        } DisplaySettingsState;

        typedef enum 
        {
            Animate,
            Animation_InitialState
        } DisplayAnimationsState;

        typedef enum 
        {
            Monitoring = 0,
            Sensor_InitialState
        } DisplaySensorState;

    private:
        DriverLayer::DisplayDriver &driver;
        ButtonManager &m_btnManager;
        DisplayState currentState;
        DisplayMainMenuState currentMenuState;
        DisplayMainMenuState previousMenuState;
        DisplaySensorState   currentSensorState;
        DisplaySettingsState currentSettingsState;
        DisplayAnimationsState currentAnimationsState;
        ButtonManager::ButtonState previousbuttonState;
        uint8_t currentMenuIndex = 0;
        uint8_t menuIndex = 0;    
        bool backToMenu = 0;
    };
}