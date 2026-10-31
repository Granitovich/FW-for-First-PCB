#pragma once
#include <cstdint>

namespace Application
{
    class StorageManager 
    {

    public:
        StorageManager() {}

        typedef struct 
        {
            uint16_t brightLevel;
            uint16_t animationTime;
            uint16_t sensorTime;
            uint16_t currentMode;
            uint16_t versionMajor;
            uint16_t versionMinor;
            uint16_t versionPatch;
        }SettingsData;

        bool restoreSettings();
        bool writeSettings(SettingsData& data);
        SettingsData getSettings() { return settingsData; }

      

        typedef enum 
        {
            Brightness_10 = 10,
            Brightness_25 = 25,
            Brightness_50 = 50,
            Brightness_75 = 75,
            Brightness_100 = 100,
        } BrightnessModes;

        typedef enum 
        {
            AnimationTime_5s  = 5,
            AnimationTime_10s = 10,
            AnimationTime_30s = 30,
            AnimationTime_45s = 45,
            AnimationTime_60s = 60,
        } AnimationTimeModes;

        typedef enum
        {
            SensorTime_5s  = 5,
            SensorTime_10s = 10,
            SensorTime_30s = 30,
            SensorTime_45s = 45,
            SensorTime_60s = 60,
        } SensorTimeModes;

        typedef enum 
        {
            SettingsMode  = 1,
            AnimationMode = 2,
            SensorMode    = 3,

        } CurrentModes;

    
        typedef enum
        {
            BrightnessAddress    = 0x01,
            AnimationTimeAddress = 0x02,
            SensorTimeAddress    = 0x03,
            CurrentModeAddreess  = 0x04,
            CurrentVersionMajor  = 0x05,
            CurrentVersionMinor  = 0x06,
            CurrentVersionParch  = 0x07,
        } VirtualAddress;

    private:
        bool restoreDefaultSettings();
        bool verifyData(SettingsData& data);
        SettingsData settingsData;
    };
}