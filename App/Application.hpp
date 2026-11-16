#pragma once

class App
{
public:
    // Singleton pattern
    static App &getInstance()
    {
        static App instance;
        return instance;
    }
    bool init();
    void run();

private:
bool initialized = false;
    
};