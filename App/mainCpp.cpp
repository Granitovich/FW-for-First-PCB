#include "main.h"

#include "Application.hpp"

#ifdef __cplusplus
extern "C"
{
#endif
    void mainCpp(void)
    {
        App &app = App::getInstance();
        
        app.run();
        
    }
    void handleErrorCpp(void)
    {
        Error_Handler();
    }
#ifdef __cplusplus
}
#endif