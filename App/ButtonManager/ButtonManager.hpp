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
            ShortPress = 1,
            LongPress,
            NoPress,
            Pressing
        } ButtonState;

        ButtonManager(bool& buttonChangedPressed, bool& buttonSetPressed) : m_buttonChangedPressed(buttonChangedPressed), m_buttonSetPressed(buttonSetPressed) {}
        ButtonState GetChangedButtonState() { return currentChangedButtonState; }
        ButtonState GetSetButtonState()     { return currentSetButtonState;     }
        void run();
        void updateButtonState(ButtonState buttonState, GPIO_TypeDef* buttonPort, uint32_t buttonPin, uint32_t& buttonPressTime, ButtonState& currentState, bool& m_buttonPressed);

    private:
          
        ButtonState currentChangedButtonState = NoPress;
        ButtonState currentSetButtonState     = NoPress;        

        const uint32_t shortPressThreshold = 1000; // 1 second threshold for short press
        const uint32_t longPressThreshold = 2000; // 2 seconds threshold for long press

        uint32_t buttonChangedPressTime = 0;
        uint32_t buttonSetPressTime     = 0;

        bool& m_buttonChangedPressed;
        bool& m_buttonSetPressed;

        uint32_t buttonChangedPin = BUTTON_CHANGE_Pin;
        uint32_t buttonSetPin    = BUTTON_SET_Pin;
        GPIO_TypeDef* buttonChangedPort = BUTTON_CHANGE_GPIO_Port;
        GPIO_TypeDef* buttonSetPort     = BUTTON_SET_GPIO_Port;
    };
}