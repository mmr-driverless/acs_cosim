#ifndef CAR_CONTROLS_H
#define CAR_CONTROLS_H

class CarControls {
public:
	bool gearUp;
	bool gearDn;
	bool drs;
	bool kers;
	char* undefined0[0x20];
	float gas;
	float brake;
	float steer;
	float clutch;
};

#endif // CAR_CONTROLS_H

