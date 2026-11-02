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

bool Application::SDCardManager::openFile(const char* filename) 
{
    FRESULT res;
    res = f_open(&file, filename, FA_READ);
    if( FR_OK != res)
    {
        return false;
    }
    return true;
    
}
bool Application::SDCardManager::readImage(uint16_t* imageBuffer, uint32_t bufferSize) 
{
    FRESULT res;
    res = f_read(&file, imageBuffer, bufferSize, &br);
    if (res != FR_OK || br != bufferSize)
    {
        return false;
    }
    return true;
}
bool Application::SDCardManager::readFile(char* fileBuffer) 
{
    FRESULT res;
    res = f_read(&file, fileBuffer,  f_size(&file), &br);
    if (res != FR_OK || br != f_size(&file))
    {
        return false;
    }
    return true;
}
bool Application::SDCardManager::seekFile(const char* filename, uint32_t offset) 
{
    FRESULT res;
    res = f_lseek(&file, offset);
    if (res != FR_OK)
    {
        return false;
    }
    return true;
}

bool Application::SDCardManager::closeFile() 
{
    FRESULT res;
    res = f_close(&file);
    if (res != FR_OK)
    {
        return false;
    }
    return true;
}
