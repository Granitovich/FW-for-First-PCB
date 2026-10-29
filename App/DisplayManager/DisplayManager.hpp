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
        bool createSensor();
        bool run();

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
            InitialState
        } DisplayMainMenuState;

    private:
        DriverLayer::DisplayDriver &driver;
        ButtonManager &m_btnManager;
        DisplayState currentState;
        DisplayMainMenuState currentMenuState;
        uint8_t currentMenuIndex = 0;
        uint8_t menuIndex = 0;    
        uint8_t sensorIndex = 0;
    };
}