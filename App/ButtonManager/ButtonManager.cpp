#include "ButtonManager.hpp" 


void Application::ButtonManager::updateButtonState(ButtonState buttonState, uint32_t buttonPin, uint32_t& buttonPressTime, ButtonState& currentState, bool& m_buttonPressed)
{
    if (m_buttonPressed || HAL_GPIO_ReadPin(BUTTON_CHANGE_GPIO_Port, buttonPin) == GPIO_PIN_RESET)
    {
        if(currentState == NoPress)
        {
            buttonPressTime = HAL_GetTick();
        }
    
        if(currentState == Pressing && (HAL_GetTick() - buttonPressTime) > longPressThreshold)
        {
            currentState = LongPress;
        }

        if(currentState != LongPress)
        {
            currentState = Pressing;
        }

        m_buttonPressed = false; // Reset the button pressed flag after processing
        
    }
    else
    {
        if(currentState == Pressing && (HAL_GetTick() - buttonPressTime) < shortPressThreshold && buttonPressTime != 0)
        {
            currentState = ShortPress;
        }
        else
        {
            currentState = NoPress;
        }
    }
}
void Application::ButtonManager::run()
{
    updateButtonState(currentChangedButtonState, buttonChangedPin, buttonChangedPressTime, currentChangedButtonState, m_buttonChangedPressed);
    updateButtonState(currentSetButtonState,     buttonSetPin,     buttonSetPressTime,     currentSetButtonState,     m_buttonSetPressed);
}