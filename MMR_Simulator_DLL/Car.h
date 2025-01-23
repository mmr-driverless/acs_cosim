#include "CarControls.h"
#ifndef CAR_H
#define CAR_H

class Car {
public:
    char* undefined0[0x140];
	CarControls controls;
    char* undefined1[0x172];
    float mass;
};

#endif // CAR_H
