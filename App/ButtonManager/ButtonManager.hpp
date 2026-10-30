#pragma once
#include <cstdint>
#include "main.h"
namespace Application
{
    class ButtonManager 
    {

    public:
        typedef enum 
        {
            ShortPress,
            LongPress,
            NoPress,
            Pressing
        } ButtonState;

        ButtonManager(bool& buttonPressed) : m_buttonPressed(buttonPressed) {}
        ButtonState GetButtonState() { return currentState; }
        void updateButtonState();

    private:
          
        ButtonState currentState = NoPress;
        const uint32_t shortPressThreshold = 1000; // 1 second threshold for short press
        const uint32_t longPressThreshold = 2000; // 3 seconds threshold for long press
        uint32_t buttonPressTime = 0;
        bool& m_buttonPressed;    
        uint32_t buttonPin = BUTTON_CHANGE_Pin;
    };
}