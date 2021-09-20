#pragma once

#define UID_WHITE  0xffffff
#define UID_75GREY 0xbfbfbf
#define UID_50GREY 0x7f7f7f
#define UID_25GREY 0x3f3f3f
#define UID_BLACK  0x000000
#define UID_BLUE   0x0000ff
#define UID_CYAN   0x00ffff
#define UID_GREEN  0x00ff00
#define UID_YELLOW 0xffff00
#define UID_ORANGE 0xff7f00
#define UID_RED    0xff0000
#define UID_PURPLE 0xff00ff
#define _25P      +0x3f000000
#define _50P      +0x7f000000
#define _75P      +0xbf000000
#define _100P     +0xff000000

#define PI 3.14159265
#define toDeg *180/PI
#define toRad *PI/180

#include <vector>
#include <math.h>

struct Object;

typedef void(*basicFunc)(Object*);

typedef struct vector2
{
	int x, y;
};