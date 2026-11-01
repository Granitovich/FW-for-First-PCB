#include "StorageManager.hpp"
#include "DisplayDriver/Constants/Version.h"
#include "eeprom_emul.h"
#include "eeprom_emul_types.h"
#include "main.h"

bool Application::StorageManager::restoreDefaultSettings() {
  settingsData.animationTime = AnimationTimeModes::AnimationTime_10s;
  settingsData.brightLevel = BrightnessModes::Brightness_10;
  settingsData.sensorTime = SensorTimeModes::SensorTime_10s;
  settingsData.currentMode = CurrentModes::AnimationMode;

  settingsData.versionMajor = version.MAJOR;
  settingsData.versionMinor = version.MINOR;
  settingsData.versionPatch = version.PATCH;

  if (!writeSettings(settingsData)) {
    return false;
  }
  return true;
}

bool Application::StorageManager::restoreSettings() {
  HAL_FLASH_Unlock();
  EE_Init(EE_FORCED_ERASE);

  if (EE_OK != EE_ReadVariable16bits(VirtualAddress::BrightnessAddress,
                                     &settingsData.brightLevel) ||
      EE_OK != EE_ReadVariable16bits(VirtualAddress::AnimationTimeAddress,
                                     &settingsData.animationTime) ||
      EE_OK != EE_ReadVariable16bits(VirtualAddress::SensorTimeAddress,
                                     &settingsData.sensorTime) ||
      EE_OK != EE_ReadVariable16bits(VirtualAddress::CurrentModeAddreess,
                                     &settingsData.currentMode) ||
      EE_OK != EE_ReadVariable16bits(VirtualAddress::CurrentVersionMajor,
                                     &settingsData.versionMajor) ||
      EE_OK != EE_ReadVariable16bits(VirtualAddress::CurrentVersionMinor,
                                     &settingsData.versionMinor) ||
      EE_OK != EE_ReadVariable16bits(VirtualAddress::CurrentVersionParch,
                                     &settingsData.versionPatch) ||
      !verifyData(settingsData)) 
      {
    EE_Format(EE_FORCED_ERASE);
    EE_Init(EE_FORCED_ERASE);

    restoreDefaultSettings();
  }

  return true;
}

bool Application::StorageManager::writeSettings(SettingsData &data) {
  if (data.brightLevel != 0) {
    settingsData.brightLevel = data.brightLevel;

    if (EE_OK != EE_WriteVariable16bits(VirtualAddress::BrightnessAddress,
                                        settingsData.brightLevel)) {
      return false;
    }
  }

  if (data.animationTime != 0) {
    settingsData.animationTime = data.animationTime;
    if (EE_OK != EE_WriteVariable16bits(VirtualAddress::AnimationTimeAddress,
                                        settingsData.animationTime)) {
      return false;
    }
  }

  if (data.sensorTime != 0) {
    settingsData.sensorTime = data.sensorTime;
    if (EE_OK != EE_WriteVariable16bits(VirtualAddress::SensorTimeAddress,
                                        settingsData.sensorTime)) {
      return false;
    }
  }

  if (data.currentMode != 0) {
    settingsData.currentMode = data.currentMode;
    if (EE_OK != EE_WriteVariable16bits(VirtualAddress::CurrentModeAddreess,
                                        settingsData.currentMode)) {
      return false;
    }
  }
  if (data.versionMajor != 0) {
    settingsData.versionMajor = data.versionMajor;
    if (EE_OK != EE_WriteVariable16bits(VirtualAddress::CurrentVersionMajor,
                                        settingsData.versionMajor)) {
      return false;
    }
  }
  if (data.versionMinor != 0) {
    settingsData.versionMinor = data.versionMinor;
    if (EE_OK != EE_WriteVariable16bits(VirtualAddress::CurrentVersionMinor,
                                        settingsData.versionMinor)) {
      return false;
    }
  }
  if (data.versionPatch != 0) {
    settingsData.versionPatch = data.versionPatch;
    if (EE_OK != EE_WriteVariable16bits(VirtualAddress::CurrentVersionParch,
                                        settingsData.versionPatch)) {
      return false;
    }
  }

  return true;
}

bool Application::StorageManager::verifyData(SettingsData &data) {

  if ((data.brightLevel != BrightnessModes::Brightness_10) &&
      (data.brightLevel != BrightnessModes::Brightness_25) &&
      (data.brightLevel != BrightnessModes::Brightness_50) &&
      (data.brightLevel != BrightnessModes::Brightness_75) &&
      (data.brightLevel != BrightnessModes::Brightness_100)) {
    return false;
  }

  if ((data.animationTime != AnimationTimeModes::AnimationTime_5s) &&
      (data.animationTime != AnimationTimeModes::AnimationTime_10s) &&
      (data.animationTime != AnimationTimeModes::AnimationTime_30s) &&
      (data.animationTime != AnimationTimeModes::AnimationTime_45s) &&
      (data.animationTime != AnimationTimeModes::AnimationTime_60s)) {
    return false;
  }

  if ((data.sensorTime != SensorTimeModes::SensorTime_5s) &&
      (data.sensorTime != SensorTimeModes::SensorTime_10s) &&
      (data.sensorTime != SensorTimeModes::SensorTime_30s) &&
      (data.sensorTime != SensorTimeModes::SensorTime_45s) &&
      (data.sensorTime != SensorTimeModes::SensorTime_60s)) {
    return false;
  }

  if ((data.currentMode != CurrentModes::SettingsMode) &&
      (data.currentMode != CurrentModes::AnimationMode) &&
      (data.currentMode != CurrentModes::SensorMode)) {
    return false;
  }

  if ((data.versionMajor != version.MAJOR) &&
      (data.currentMode != version.MINOR) &&
      (data.currentMode != version.PATCH)) {
    return false;
  }

  return true;
}

etl::string<Application::StorageManager::versionBufferSize>
Application::StorageManager::getVersion() const {
  etl::string<versionBufferSize> ver;

  char versionBuf[versionBufferSize];
  snprintf(versionBuf, sizeof(versionBuf), "%u.%u.%u",
           settingsData.versionMajor, settingsData.versionMinor,
           settingsData.versionPatch);
  ver.repair();

  ver = versionBuf;
  return ver;
}

etl::string<Application::StorageManager::versionBufferSize>
Application::StorageManager::getBrightness() const {
  etl::string<versionBufferSize> ver;

  char versionBuf[versionBufferSize];

  switch (settingsData.brightLevel) {
  case BrightnessModes::Brightness_10: {
    snprintf(versionBuf, sizeof(versionBuf), "10%%");
    break;
  }
  case BrightnessModes::Brightness_25: {
    snprintf(versionBuf, sizeof(versionBuf), "25%%");
    break;
  }
  case BrightnessModes::Brightness_50: {
    snprintf(versionBuf, sizeof(versionBuf), "50%%");
    break;
  }
  case BrightnessModes::Brightness_75: {
    snprintf(versionBuf, sizeof(versionBuf), "75%%");
    break;
  }
  case BrightnessModes::Brightness_100: {
    snprintf(versionBuf, sizeof(versionBuf), "100%%");
  }
  }
  ver.repair();

  ver = versionBuf;
  return ver;
}

etl::string<Application::StorageManager::versionBufferSize>
Application::StorageManager::getAnimationTime() const {
  etl::string<versionBufferSize> ver;

  char versionBuf[versionBufferSize];

  switch (settingsData.animationTime) {
  case AnimationTimeModes::AnimationTime_5s: {
    snprintf(versionBuf, sizeof(versionBuf), "5s");
    break;
  }
  case AnimationTimeModes::AnimationTime_10s: {
    snprintf(versionBuf, sizeof(versionBuf), "10s");
    break;
  }
  case AnimationTimeModes::AnimationTime_30s: {
    snprintf(versionBuf, sizeof(versionBuf), "30s");
    break;
  }
  case AnimationTimeModes::AnimationTime_45s: {
    snprintf(versionBuf, sizeof(versionBuf), "45s");
    break;
  }
  case AnimationTimeModes::AnimationTime_60s: {
    snprintf(versionBuf, sizeof(versionBuf), "60s");
    break;
  }
  }
  ver.repair();

  ver = versionBuf;
  return ver;
}

etl::string<Application::StorageManager::versionBufferSize>
Application::StorageManager::getSensorTime() const {
  etl::string<versionBufferSize> ver;

  char versionBuf[versionBufferSize];

  switch (settingsData.sensorTime) {
  case SensorTimeModes::SensorTime_5s: {
    snprintf(versionBuf, sizeof(versionBuf), "5s");
    break;
  }
  case SensorTimeModes::SensorTime_10s: {
    snprintf(versionBuf, sizeof(versionBuf), "10s");
    break;
  }
  case SensorTimeModes::SensorTime_30s: {
    snprintf(versionBuf, sizeof(versionBuf), "30s");
    break;
  }
  case SensorTimeModes::SensorTime_45s: {
    snprintf(versionBuf, sizeof(versionBuf), "45s");
    break;
  }
  case SensorTimeModes::SensorTime_60s: {
    snprintf(versionBuf, sizeof(versionBuf), "60s");
    break;
  }
  }
  ver.repair();

  ver = versionBuf;
  return ver;
}