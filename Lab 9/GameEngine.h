#ifndef GAMEENGINE_H_
#define GAMEENGINE_H_

struct Entity
{
    uint8_t x;  //x position 0-128
    uint8_t y;  //y position 0-160
    int8_t vx;  //x velocity
    int8_t vy;  //y velocity
    int8_t ax;  //x acceleration
    int8_t ay;  //y acceleration
    uint8_t health; //for if we have bad guys
    uint8_t visible;    //1 for visible, 0 for invisible
    uint8_t h, w;    //height and width of sprite
    uint8_t xOld, yOld;
};
typedef struct Entity Entity_t;

uint32_t GameConvert(uint32_t input);

void DoodlerInit(Entity_t *doodler);

void PlatformsInit();

void UpdateDoodlerPosition(Entity_t *doodler);

void UpdateDoodlerSpeed(Entity_t *doodler);

void SetOldPosition(Entity_t *entity);

void CheckForCollision(Entity_t *doodler);

#endif
