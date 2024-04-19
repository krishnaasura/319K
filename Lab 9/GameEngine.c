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

#define RED (1 << 17)
#define YELLOW (1 << 28)
#define GREEN (1 << 31)

#define UP (1<<24)
#define DOWN (1<<26)
#define LEFT (1<<25)
#define RIGHT (1<<27)

extern uint32_t *ADCData;

uint32_t GameConvert(uint32_t input)
{
    int32_t val;
    val = input >> 9;
    val -= 4;
    if(val < 0) val++;
    return val;
}

void DoodlerInit(Entity_t *doodler)
{
    doodler->x = 64;
    doodler->y = 30;
    doodler->vx = 0;
    doodler->vy = 0;
    doodler->ax = 0;
    doodler->ay = 10;
    doodler->visible = 1;
    doodler->w = 19;
    doodler->h = 21;
}

void UpdateDoodlerPosition(Entity_t *doodler)
{
    int8_t xtemp = doodler->x + doodler->vx;

    if(xtemp < 0) doodler->x = 109;
    else if(xtemp > 109) doodler->x = 0;
    else doodler->x = xtemp;

    doodler->vy = doodler->vy + doodler->ay;
    int8_t ytemp = doodler->y + doodler->vy;

    if(ytemp >= 159)
    {
        doodler->vy = -80;
    }
    doodler->y = ytemp;
}

void UpdateDoodlerSpeed(Entity_t *doodler)
{
    doodler->vx = ADCData;
}
