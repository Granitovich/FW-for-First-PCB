#include "StorageManager.hpp"
#include "main.h"
#include "eeprom_emul.h"

bool Application::StorageManager::restoreDefaultSettings()
{
    settingsData.animationTime = AnimationTimeModes::AnimationTime_10s;
    settingsData.brightLevel    = BrightnessModes::Brightness_10;
    settingsData.sensorTime    = SensorTimeModes::SensorTime_10s;
    settingsData.currentMode   = CurrentModes::AnimationMode;

    if(!writeSettings(settingsData))
    {
        return false;
    }
    return true;
}

bool Application::StorageManager::restoreSettings()
{
    EE_Init(EE_FORCED_ERASE);

    if(!EE_ReadVariable16bits(VirtualAddress::BrightnessAddress, &settingsData.brightLevel)      ||
       !EE_ReadVariable16bits(VirtualAddress::AnimationTimeAddress, &settingsData.animationTime) ||
       !EE_ReadVariable16bits(VirtualAddress::SensorTimeAddress, &settingsData.sensorTime)       ||
       !EE_ReadVariable16bits(VirtualAddress::CurrentModeAddreess, &settingsData.currentMode))
    {
        EE_Format(EE_FORCED_ERASE);
        EE_Init(EE_FORCED_ERASE);
        
        restoreDefaultSettings();

    }
    return true;
}
bool Application::StorageManager::writeSettings(SettingsData& data)
{
    if(data.brightLevel != 0)
    {
        settingsData.brightLevel = data.brightLevel;

        if(!EE_WriteVariable16bits(VirtualAddress::BrightnessAddress, settingsData.brightLevel)) 
        {
            return false;
        }
    }

    if(data.animationTime != 0)
    {
        settingsData.animationTime = data.animationTime;
        if(!EE_WriteVariable16bits(VirtualAddress::AnimationTimeAddress, settingsData.animationTime)) 
        {
            return false;
        }
    }

    if(data.sensorTime != 0)
    {
        settingsData.sensorTime = data.sensorTime;
        if(!EE_WriteVariable16bits(VirtualAddress::SensorTimeAddress, settingsData.sensorTime)) 
        {
            return false;
        }
    }

    if(data.currentMode != 0)
    {
        settingsData.currentMode =  data.currentMode;
        if(!EE_WriteVariable16bits(VirtualAddress::CurrentModeAddreess, settingsData.currentMode)) 
        {
            return false;
        }
    }

    return true;
}