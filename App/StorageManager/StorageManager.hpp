#pragma once
#include <cstdint>
#include <etl/vector.h>
#include <etl/to_string.h>
#include <stdint.h>

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

        static constexpr uint8_t versionBufferSize = 18;
        etl::string<versionBufferSize> getVersion() const;
        etl::string<versionBufferSize> getBrightness() const;
        etl::string<versionBufferSize> getAnimationTime() const;
        etl::string<versionBufferSize> getSensorTime() const;
    
        typedef enum 
        {
            Brightness_10 = 1,
            Brightness_25 = 2,
            Brightness_50 = 3,
            Brightness_75 = 4,
            Brightness_100 = 5,
            Brightness_Count = 5,
        } BrightnessModes;

        typedef enum 
        {
            AnimationTime_10s  = 1,
            AnimationTime_20s = 2,
            AnimationTime_30s = 3,
            AnimationTime_45s = 4,
            AnimationTime_60s = 5,
            AnimationTime_Count = 5,
        } AnimationTimeModes;

        typedef enum
        {
            SensorTime_10s  = 1,
            SensorTime_20s = 2,
            SensorTime_30s = 3,
            SensorTime_45s = 4,
            SensorTime_60s = 5,
            SensorTime_Count = 5,
        } SensorTimeModes;

        typedef enum 
        {
            SettingsMode  = 1,
            AnimationMode = 2,
            SensorMode    = 3,
            CurrentMode_Count = 3,

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
        etl::string<versionBufferSize> versionBuffer;
        SettingsData settingsData;
    };
}