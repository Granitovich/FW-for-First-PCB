#pragma once

#include "ButtonManager/ButtonManager.hpp"
#include "DisplayDriver/DisplayDriver.hpp"
#include "StorageManager/StorageManager.hpp"


namespace Application {
class DisplayManager {

public:
  DisplayManager(DriverLayer::DisplayDriver &dispDriver,
                 ButtonManager &btnManager, StorageManager &storage)
      : driver(dispDriver), m_btnManager(btnManager), m_storage(storage),
        currentState(DisplayState::MAIN_MENU) {}
  bool createInit();
  bool createMenu();
  bool createSettings();
  bool createAnimation();
  bool createSensor();
  bool run();
  bool mainMenuHadle(ButtonManager::ButtonState buttonState);
  bool settingsHandle(ButtonManager::ButtonState buttonState);
  bool animationHandle(ButtonManager::ButtonState buttonState);
  bool sensorHandle(ButtonManager::ButtonState buttonState);
  bool brightnessHandle();

  typedef enum { ANIMATION, MAIN_MENU, SETTINGS, SENSOR_DATA } DisplayState;

  typedef enum {
    Settings = 0,
    Animation,
    Sensoring,
    StateCount,
    MainMenu_InitialState
  } DisplayMainMenuState;

  typedef enum {
    Brightness = 0,
    AnimationTime,
    SensTime,
    Settings_Count,
    Settings_InitialState
  } DisplaySettingsState;

  typedef enum { Animate, Animation_InitialState } DisplayAnimationsState;

  typedef enum { Monitoring = 0, Sensor_InitialState } DisplaySensorState;

private:
  DriverLayer::DisplayDriver &driver;
  ButtonManager &m_btnManager;
  StorageManager &m_storage;
  DisplayState currentState;
  DisplayMainMenuState currentMenuState;
  DisplayMainMenuState previousMenuState;
  DisplaySensorState currentSensorState;
  DisplaySettingsState currentSettingsState;
  DisplayAnimationsState currentAnimationsState;
  ButtonManager::ButtonState previousChangedButtonState;
  ButtonManager::ButtonState previousSetButtonState;
  uint8_t currentMenuIndex = 0;
  uint8_t currentSettingsIndex = 0;
  uint8_t menuIndex = 0;
  bool backToMenu = 0;
  uint8_t animationIndex = 0;
  uint8_t sensorIndex = 0;
  uint8_t brightnessIndex = 0;
  uint8_t currentBrightnessIndex = 0;
};
} // namespace Application