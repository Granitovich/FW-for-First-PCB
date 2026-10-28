#include "DisplayDriver/Constants/fonts.h"
#include "main.h"
#include "Application.hpp"
#include "DisplayDriver/DisplayDriver.hpp"
#include "DisplayDriver/Constants/Images.h"
#include "DisplayDriver/Constants/Fonts.h"
extern SPI_HandleTypeDef hspi3;


// This is the C++ code that will run in the main loop.
bool App::init()
{
    DriverLayer::DisplayDriver::DriverGpio gpios;
    gpios =
    {
        .resetPort   = DISP_RES_GPIO_Port,
        .resetPin    = DISP_RES_Pin,
        .dataCmdPort = DISP_DC_GPIO_Port,
        .dataCmdPin  = DISP_DC_Pin,
        .csPort      = DISP_CS_GPIO_Port,
        .csPin       = DISP_CS_Pin,
        .ledPort     = DISP_LED_GPIO_Port,
        .ledPin      = DISP_LED_Pin
    
    };
   DriverLayer::DisplayDriver display(&hspi3, gpios);

   display.init();
   display.setRotation(DriverLayer::DisplayDriver::DisplayDriverRotation::Portrait);
   display.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   display.drawString(20, 10, "MemoryHub", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   display.drawString(10, 40, "Created by", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   display.drawString(5, 60, "Granitovich", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   display.drawString(20, 80, "With Love", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::RED, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   display.drawString(20, 100, "to Family", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::RED, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   display.drawString(5, 130, "Version 1.0", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   HAL_Delay(3000);
   display.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   //display.drawLine(0,0, 10, 0, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN);
   //display.drawFastHLine(10, 0, 100, DriverLayer::DisplayDriver::DisplayDriverColor::RED);
   //display.drawFastVLine(10, 0, 100, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN);
   display.drawRoundRect(10, 10, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
   display.drawString(20, 20, "Settings", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

   display.drawRoundRect(10, 60, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
   display.drawString(15, 70, "Animation", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
  
   //display.drawImage(Images::MemoryCardErrorPortrait);
    // Place your initialization code here.
    return true;
}

void App::run()
{
    
    // Place your main application loop logic here.
    HAL_Delay(100);
}