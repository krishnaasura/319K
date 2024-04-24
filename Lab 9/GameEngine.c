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
extern uint32_t switchDataOld;
extern uint32_t switchData;

extern uint8_t GAMEEND;
extern uint8_t MAINMENU;
extern uint8_t GAMESTART;
extern uint8_t PAUSE;
extern uint8_t STORE;

uint32_t Score = 0;

uint8_t PlatformCollisionFlag = 0;

uint32_t GameConvert(uint32_t input)
{
    int32_t val;
    val = input >> 9;
    val -= 4;
    if(val < 0) val++;
    return val*2;
}

void DoodlerInit(Entity_t *doodler, uint16_t x, uint16_t y)
{
    doodler->x = x;
    doodler->y = y;    //90 spaces between platform and doodler head
    doodler->vx = 0;
    doodler->vy = 0;
    doodler->ax = 0;
    doodler->ay = 4;
    doodler->visible = 1;
    doodler->w = 19;
    doodler->h = 21;
    doodler->xOld = doodler->x;
    doodler->yOld = doodler->y;
    doodler->FIXEDPTy = (doodler->y)*10;
}

void PlatformsInit()
{
    for(uint8_t i = 0; i < NumOfPlatforms; i++)
    {
        Platforms[i].x = Random(101);
        Platforms[i].y = Random(160);
        //Platforms[i].x = 55;    //for debug
        //Platforms[i].y = 160;   //for debug - 153 is max for contact
        Platforms[i].w = 27;
        Platforms[i].h = 7;
        Platforms[i].vy = 0;
        Platforms[i].ay = 0;
        Platforms[i].FIXEDPTy = (Platforms[i].y)*10;

        if(i != 0)
        {
            for(uint8_t j = 0; j < NumOfPlatforms; j++)
            {
                if(i != j)
                {
                    int16_t xDiff = Platforms[i].x - Platforms[j].x;
                    int16_t yDiff = Platforms[i].y - Platforms[j].y;
                    if(xDiff < 0) xDiff *= -1;
                    if(yDiff < 0) yDiff *= -1;

                    while((xDiff < 35) && (yDiff < 21))
                    {
                        Platforms[i].x = Random(101);
                        Platforms[i].y = Random(160);
                        xDiff = Platforms[i].x - Platforms[j].x;
                        yDiff = Platforms[i].y - Platforms[j].y;
                        if(xDiff < 0) xDiff *= -1;
                        if(yDiff < 0) yDiff *= -1;
                    }

                    Platforms[i].FIXEDPTy = (Platforms[i].y)*10;
                }


            }
        }

    }
}

void UpdateDoodlerPosition(Entity_t *doodler)
{
    SetOldPosition(doodler);

    int16_t xtemp = doodler->x + doodler->vx;    //x position + x velocity

    //for screen wrap
    if(xtemp < 0) doodler->x = 109;
    else if(xtemp > 109) doodler->x = 0;
    else doodler->x = xtemp;

    doodler->vy = doodler->vy + doodler->ay;    //y velocity += y acceleration
    doodler->FIXEDPTy = doodler->FIXEDPTy + doodler->vy;   //y position += y velocity

    //if doodler hits the ground, shoot him back up (REMOVE LATER)
    /*if(ytemp >= 159)
    {
        doodler->vy = -13;
    }
    */

    //if(doodler->FIXEDPTy > 1650) doodler->FIXEDPTy = 0;
    doodler->y = (doodler->FIXEDPTy)/10;

    if((doodler->FIXEDPTy > 1650) && (doodler->FIXEDPTy < 60000)) GameEnd(doodler);

}

void UpdateDoodlerSpeed(Entity_t *doodler)
{
    doodler->vx = GameConvert(ADCData);
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
                //if(((Platforms[i].y + 7) > (doodler->y + 15)) && ((doodler->y + 15) > Platforms[i].y))
                //if(((Platforms[i].y + 14) > (doodler->y + 21)) && ((doodler->y + 21) > (Platforms[i].y + 7)))
                if((Platforms[i].y > doodler->y) && (doodler->y > (Platforms[i].y - 8)))
                {
                    Sound_Start(1, 3500);   //Pop sound
                    doodler->vy = -55;
                    PlatformCollisionFlag = 1;
                }
            }
        }
     }
}

/*
 *
 * instead of vx being -128 to 127, have it be -128.0 to 127.0 by treating it
 * as -1280 and 1270, and incrementing by 1 (which is 0.1), then divide by 10 to get
 * actual value.
 * Say:
 * VxFixed: 1, 1.3, 1.7, 2.1, 2.6, 3.1, 3.8, 4.3, 5
 * VxOut:   1  1    1    2    2    3    3    4    5
 */


void UpdatePlatforms()
{
    for(uint8_t i = 0; i < NumOfPlatforms; i++)
    {
        Platforms[i].xOld = Platforms[i].x;
        Platforms[i].yOld = Platforms[i].y;
        Platforms[i].vy = Platforms[i].vy + Platforms[i].ay;
        Platforms[i].FIXEDPTy = Platforms[i].FIXEDPTy + Platforms[i].vy;
        Platforms[i].y = (Platforms[i].FIXEDPTy)/10;
    }



    if(PlatformCollisionFlag == 1)
    {
        for(uint8_t i = 0; i < NumOfPlatforms; i++)
        {
            Platforms[i].vy = 120;
            Platforms[i].ay = -7;
        }
        PlatformCollisionFlag = 0;
    }



    for(uint8_t i = 0; i < NumOfPlatforms; i++)
    {
        if(Platforms[i].FIXEDPTy > 1650)
        {
            Platforms[i].FIXEDPTy = 0;
            Platforms[i].y = 0;
            Platforms[i].x = Random(101);
            for(uint8_t j = 0; j < NumOfPlatforms; j++)
            {
                if(i != j)
                {
                    int16_t xDiff = Platforms[i].x - Platforms[j].x;
                    int16_t yDiff = Platforms[i].y - Platforms[j].y;
                    if(xDiff < 0) xDiff *= -1;
                    if(yDiff < 0) yDiff *= -1;

                    while((xDiff < 35) && (yDiff < 21))
                    {
                        Platforms[i].x = Random(101);
                        xDiff = Platforms[i].x - Platforms[j].x;
                        if(xDiff < 0) xDiff *= -1;
                    }
                }
            }
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



    UpdateScore();

}

void UpdateScore()
{
    if(Platforms[0].vy >= 0)
    {
        Score += (Platforms[0].vy / 10);
    }
}

void RemovePlatforms()
{
    for(uint8_t i = 0; i < NumOfPlatforms; i++)
    {
        Platforms[i].x = 0;
        Platforms[i].y = 0;
        Platforms[i].w = 27;
        Platforms[i].h = 7;
        Platforms[i].vy = 0;
        Platforms[i].ay = 0;
        Platforms[i].FIXEDPTy = (Platforms[i].y)*10;
    }
}

void RemoveDoodler(Entity_t *doodler)
{
    doodler->x = 0;
    doodler->y = 0;    //90 spaces between platform and doodler head
    doodler->vx = 0;
    doodler->vy = 0;
    doodler->ax = 0;
    doodler->ay = 0;
    doodler->visible = 1;
    doodler->xOld = doodler->x;
    doodler->yOld = doodler->y;
    doodler->FIXEDPTy = (doodler->y)*10;
}

void GameEnd(Entity_t *doodler)
{
    Sound_Start(2, 5000);   //Fall sound

    GAMESTART = 0;
    MAINMENU = 1;
    GAMEEND = 1;

    RemovePlatforms();
    RemoveDoodler(doodler);
}

void CheckForPause(Entity_t *doodler)
{
    if(STORE == 1)
    {
        UnFreezeDoodler(doodler);
        STORE = 0;
    }

    if(((switchData & UP) == UP) && (switchDataOld == 0))
    {
        PAUSE = 1;
        STORE = 1;
    }
}

void CheckforUnpause(Entity_t *doodler)
{
    if(STORE == 1)
    {
        FreezeDoodler(doodler);
        STORE = 0;
    }

    if(((switchData & UP) == UP) && (switchDataOld == 0))
        {
            PAUSE = 0;
            STORE = 1;
        }
}

void FreezeDoodler(Entity_t *doodler)
{
    doodler->storedvx = doodler->vx;
    doodler->storedvy = doodler->vy;
    doodler->storeday = doodler->ay;
    doodler->vx = 0;
    doodler->vy = 0;
    doodler->ay = 0;
}

void UnFreezeDoodler(Entity_t *doodler)
{
    doodler->vx = doodler->storedvx;
    doodler->vy = doodler->storedvy;
    doodler->ay = doodler->storeday;
}

