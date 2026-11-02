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
        bool openFile(char* filename);
        bool readFile(uint16_t* imageBuffer, uint32_t bufferSize);
        bool seekFile(char* filename, uint32_t offset);
       
    private:
       FATFS fs;
       FIL  file;
	   UINT br;
       
    };
}
