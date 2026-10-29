#include "DisplayManager.hpp"

bool Application::DisplayManager::createInit() 
{

   driver.init();
   driver.setRotation(DriverLayer::DisplayDriver::DisplayDriverRotation::Portrait);
   //display.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

   //display.drawImage(Images::MemoryCardErrorPortrait);


   //HAL_Delay(5000);
   driver.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   driver.drawString(20, 10, "MemoryHub", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   driver.drawString(10, 40, "Created by", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   driver.drawString(5, 60, "Granitovich", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   driver.drawString(20, 80, "With Love", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::RED, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   driver.drawString(20, 100, "to Family", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::RED, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   driver.drawString(5, 130, "Version 1.0", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   HAL_Delay(1000);
   driver.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
   //display.drawLine(0,0, 10, 0, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN);
   //display.drawFastHLine(10, 0, 100, DriverLayer::DisplayDriver::DisplayDriverColor::RED);
   //display.drawFastVLine(10, 0, 100, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN);
   driver.drawRoundRect(10, 10, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
   driver.drawString(20, 20, "Settings", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

   driver.drawRoundRect(10, 60, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
   driver.drawString(15, 70, "Animation", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

   driver.drawRoundRect(10, 110, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
   driver.drawString(15, 120, "Sensoring", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

   return true;
}
  