#pragma once

#include "DisplayDriver/DisplayDriver.hpp" 

typedef enum 
{
    ANIMATION,
    MAIN_MENU,
    SETTINGS,
    SENSOR_DATA
} DisplayState;

namespace Application
{
    class DisplayManager 
    {

    public:
        DisplayManager(DriverLayer::DisplayDriver &dispDriver) : driver(dispDriver), currentState(DisplayState::MAIN_MENU) {}
        bool createInit();
        bool createMenu();
        bool createSensor();

    private:
        DriverLayer::DisplayDriver &driver; 
        DisplayState currentState;
        uint8_t menuIndex = 0;    
    };
}