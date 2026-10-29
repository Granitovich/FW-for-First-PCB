#include "DisplayDriver/Constants/fonts.h"
#include "main.h"
#include "Application.hpp"
#include "DisplayDriver/DisplayDriver.hpp"
#include "DisplayDriver/Constants/Images.h"
#include "DisplayDriver/Constants/Fonts.h"
#include "stm32g0xx_hal.h"
#include "DisplayManager/DisplayManager.hpp"
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

    DriverLayer::DisplayDriver driver(&hspi3, gpios);
    Application::DisplayManager display(driver);
    display.createInit();
  
   //display.drawImage(Images::MemoryCardErrorPortrait);
    // Place your initialization code here.
    return true;
}

void App::run()
{
    
    // Place your main application loop logic here.
    HAL_Delay(100);
}