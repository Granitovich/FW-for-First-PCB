#include "main.h"

#include "Application.hpp"

#ifdef __cplusplus
extern "C"
{
#endif
    void mainCpp(void)
    {
        App &app = App::getInstance();
        if (!app.init())
        {
            Error_Handler();
        }
        while (1)
        {
            app.run();
        }
    }
    void handleErrorCpp(void)
    {
        Error_Handler();
    }
#ifdef __cplusplus
}
#endif