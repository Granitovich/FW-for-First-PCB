#pragma once
#include "DisplayDriver/DisplayDriver.hpp" 


typedef enum {
    ANIMATION,
    MAIN_MENU,
    SETTINGS,
    SENSOR_DATA
} DisplayState;

class DisplayManager 
{

public:
    // Передаємо посилання на ваш існуючий драйвер у конструктор
    DisplayManager(DriverLayer::DisplayDriver &dispDriver) : driver(dispDriver), currentState(DisplayState::SPLASH_SCREEN) {}

    

private:
    DriverLayer::DisplayDriver &driver;  
    DisplayState currentState;
    uint8_t menuIndex = 0;    
};