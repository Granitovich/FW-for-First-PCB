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
   currentMenuState = static_cast<DisplayMainMenuState>(DisplayMainMenuState::InitialState);
   createMenu();

   return true;
}
bool Application::DisplayManager::run()
{
   m_btnManager.updateButtonState();
   if(m_btnManager.GetButtonState() == ButtonManager::ButtonState::ShortPress)
   {
       if(currentMenuIndex < DisplayMainMenuState::StateCount - 1)
       {
           currentMenuIndex++;
       }
       else
       {
           currentMenuIndex = 0;
       }
       currentMenuState = static_cast<DisplayMainMenuState>(currentMenuIndex);
        if(sensorIndex == 1)
      {
         currentMenuState = static_cast<DisplayMainMenuState>(DisplayMainMenuState::InitialState);
      }
       createMenu();
       sensorIndex = 0; // Reset sensorIndex when returning to the main menu
       
   }
   if(m_btnManager.GetButtonState() == ButtonManager::ButtonState::LongPress)
   {
      createSensor();
       // Handle long press event
       // For example, you can perform a specific action or navigate to a different menu
   }

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

      //driver.drawRoundRect(10, 110, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
      //driver.drawString(15, 120, "Sensoring", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
      
      break;
   }

    case DisplayMainMenuState::Sensoring:
    {
      //driver.drawRoundRect(10, 10, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
      //driver.drawString(20, 20, "Settings", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

      driver.drawRoundRect(10, 110, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
      driver.drawString(15, 120, "Sensoring", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

      driver.drawRoundRect(10, 60, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
      driver.drawString(15, 70, "Animation", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

        break;
    }

    case DisplayMainMenuState::InitialState:
    { 
      driver.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
      driver.drawRoundRect(10, 10, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
      driver.drawString(20, 20, "Settings", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::GREEN, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

      driver.drawRoundRect(10, 60, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
      driver.drawString(15, 70, "Animation", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);

      driver.drawRoundRect(10, 110, 108, 40, 3, DriverLayer::DisplayDriver::DisplayDriverColor::BLUE);
      driver.drawString(15, 120, "Sensoring", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        break;
    }
     
    
    default:
      break;
    }
    return true;
}  

bool Application::DisplayManager::createSensor()
{
    if(sensorIndex == 0)
    {
        driver.fillScreen(DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawString(0, 10, "Sensor Data", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawString(0, 40, "Temperature:", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawString(0, 70, "Humidity:", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        driver.drawString(0, 100, "Pressure: ", Font_11x18, DriverLayer::DisplayDriver::DisplayDriverColor::LIGHT_GRAY, DriverLayer::DisplayDriver::DisplayDriverColor::BLACK);
        sensorIndex = 1;
    }
    else
    {
        // Display other sensor data or handle other sensor states
    }



    return true;
}  