#include "SDCardManager.hpp"

bool Application::SDCardManager::init() 
{
    FRESULT res;
	res = f_mount(&fs, "", 1);
	if (res != FR_OK )
	{
	     return false;
	}
	return true;
}

bool Application::SDCardManager::openFile(char* filename) 
{
    FRESULT res;
    res = f_open(&file, filename, FA_READ);
    if( FR_OK != res)
    {
        return false;
    }
    return true;
    
}
bool Application::SDCardManager::readFile(uint16_t* imageBuffer, uint32_t bufferSize) 
{
    FRESULT res;
    res = f_read(&file, imageBuffer, bufferSize, &br);
    if (res != FR_OK || br != bufferSize)
    {
        return false;
    }
    return true;
}
bool Application::SDCardManager::seekFile(char* filename, uint32_t offset) 
{
    FRESULT res;
    res = f_lseek(&file, offset);
    if (res != FR_OK)
    {
        return false;
    }
    return true;
}