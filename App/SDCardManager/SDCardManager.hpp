#pragma once
#include <cstdint>
#include <etl/vector.h>
#include <etl/to_string.h>
#include <stdint.h>
#include "ff.h"

namespace Application
{
    class SDCardManager 
    {

    public:
        SDCardManager() {}

        bool init();
        bool openFile(const char* filename);
        bool readImage(uint16_t* imageBuffer, uint32_t bufferSize);
        bool readFile(char* fileBuffer);
        bool seekFile(const char* filename, uint32_t offset);
        bool closeFile();
       
    private:
      
       
    };
}
