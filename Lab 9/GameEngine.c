#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
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
#include "Lab9Main.h"

#define RED (1 << 17)
#define YELLOW (1 << 28)
#define GREEN (1 << 31)

#define UP (1<<24)
#define DOWN (1<<26)
#define LEFT (1<<25)
#define RIGHT (1<<27)

extern uint32_t ADCData;
extern Entity_t Platforms[NumOfPlatforms];

uint8_t PlatformCollisionFlag = 0;

uint32_t GameConvert(uint32_t input)
{
    int32_t val;
    val = input >> 9;
    val -= 4;
    if(val < 0) val++;
    return val*2;
}

void DoodlerInit(Entity_t *doodler)
{
    doodler->x = 64;
    doodler->y = 30;
    doodler->vx = 0;
    doodler->vy = 0;
    doodler->ax = 0;
    doodler->ay = 1;
    doodler->visible = 1;
    doodler->w = 19;
    doodler->h = 21;
    doodler->xOld = doodler->x;
    doodler->yOld = doodler->y;
}

void PlatformsInit()
{
    for(uint8_t i = 0; i < NumOfPlatforms; i++)
    {
        Platforms[i].x = Random(101);
        Platforms[i].y = Random(152);
        Platforms[i].w = 27;
        Platforms[i].h = 7;
        Platforms[i].vy = 0;
        Platforms[i].ay = 0;
    }
}

void UpdateDoodlerPosition(Entity_t *doodler)
{
    SetOldPosition(doodler);

    int8_t xtemp = doodler->x + doodler->vx;    //x position + x velocity

    //for screen wrap
    if(xtemp < 0) doodler->x = 109;
    else if(xtemp > 109) doodler->x = 0;
    else doodler->x = xtemp;

    doodler->vy = doodler->vy + doodler->ay;    //y velocity += y acceleration
    int16_t ytemp = doodler->y + doodler->vy;   //y position += y velocity

    //if doodler hits the ground, shoot him back up (REMOVE LATER)
    if(ytemp >= 159)
    {
        doodler->vy = -13;
    }
    doodler->y = ytemp;
}

void UpdateDoodlerSpeed(Entity_t *doodler)
{
    doodler->vx = ADCData;
}

void SetOldPosition(Entity_t *entity)
{
    entity->xOld = entity->x;
    entity->yOld = entity->y;
}

void CheckForCollision(Entity_t *doodler)
{
    int16_t x = doodler->x;
    int16_t y = doodler->y;
    if(doodler->vy > 0)
    {
        for(uint8_t i = 0; i < NumOfPlatforms; i++)
        {
            int16_t xDiff = doodler->x - Platforms[i].x;
            if((xDiff < 27) && (xDiff > -19))
            {
                if(((Platforms[i].y + 7) > (doodler->y + 15)) && ((doodler->y + 15) > Platforms[i].y))
                {
                    doodler->vy = -11;
                    PlatformCollisionFlag = 1;
                }
            }
        }
     }
}

void UpdatePlatforms()
{
    for(uint8_t i = 0; i < NumOfPlatforms; i++)
    {
        Platforms[i].xOld = Platforms[i].x;
        Platforms[i].yOld = Platforms[i].y;
        Platforms[i].vy = Platforms[i].vy + Platforms[i].ay;
        Platforms[i].y = Platforms[i].y + Platforms[i].vy;
    }

    if(PlatformCollisionFlag == 1)
    {
        for(uint8_t i = 0; i < NumOfPlatforms; i++)
        {
            Platforms[i].vy = 10;
            Platforms[i].ay = -1;
        }
        PlatformCollisionFlag = 0;
    }

    for(uint8_t i = 0; i < NumOfPlatforms; i++)
    {
        if(Platforms[i].y > 165)
        {
            Platforms[i].y = 0;
            Platforms[i].x = Random(101);
        }
    }

    if(Platforms[0].vy <= 0)
    {
        for(uint8_t i = 0; i < NumOfPlatforms; i++)
                {
                    Platforms[i].ay = 0;
                    Platforms[i].vy = 0;
                }
    }
}
