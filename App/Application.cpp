#include "Application.hpp"
#include "DisplayDriver/Constants/Fonts.h"
#include "DisplayDriver/Constants/Images.h"
#include "DisplayDriver/Constants/fonts.h"
#include "DisplayDriver/DisplayDriver.hpp"
#include "DisplayManager/DisplayManager.hpp"
#include "ButtonManager/ButtonManager.hpp"
#include "main.h"
#include "stm32g0xx_hal.h"
extern SPI_HandleTypeDef hspi3;
extern bool buttonPressed;

// This is the C++ code that will run in the main loop.
void App::run() 
{
  DriverLayer::DisplayDriver::DriverGpio gpios;
  gpios = {.resetPort = DISP_RES_GPIO_Port,
           .resetPin = DISP_RES_Pin,
           .dataCmdPort = DISP_DC_GPIO_Port,
           .dataCmdPin = DISP_DC_Pin,
           .csPort = DISP_CS_GPIO_Port,
           .csPin = DISP_CS_Pin,
           .ledPort = DISP_LED_GPIO_Port,
           .ledPin = DISP_LED_Pin

  };

  DriverLayer::DisplayDriver driver(&hspi3, gpios);
  Application::ButtonManager btnManager(buttonPressed);
  Application::DisplayManager display(driver, btnManager);


    display.createInit();

  while((true))
  {
    display.run();
  }


  // display.drawImage(Images::MemoryCardErrorPortrait);
  //  Place your initialization code here.
}
