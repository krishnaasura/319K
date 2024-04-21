#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/ADC1.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "GameEngine.h"
#include "images/images.h"
#include "Lab9Main.h"

extern Entity_t Platforms[NumOfPlatforms];
extern Entity_t Doodler;

void DisplayPlatforms()
{
    for(uint8_t i = 0; i < NumOfPlatforms; i++)
    {
        ST7735_DrawBitmap(Platforms[i].x, Platforms[i].y, PlatformGreen, Platforms[i].w, Platforms[i].h);
    }
}

void DisplayDoodler()
{
    ST7735_DrawBitmap(Doodler.xOld, Doodler.yOld, DoodlerBlank, Doodler.w, Doodler.h);
    ST7735_DrawBitmap(Doodler.x, Doodler.y, DoodlerRight, Doodler.w, Doodler.h);
}
