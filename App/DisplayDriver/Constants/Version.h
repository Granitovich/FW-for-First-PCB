#include <cstdint>

#define MAJOR_VERSION 1
#define MINOR_VERSION 1
#define PATCH_VERSION 1

typedef struct 
{
    uint8_t MAJOR;
    uint8_t MINOR;
    uint8_t PATCH;
} Version;

Version version =
{
    .MAJOR = MAJOR_VERSION,
    .MINOR = MINOR_VERSION,
    .PATCH = PATCH_VERSION
};