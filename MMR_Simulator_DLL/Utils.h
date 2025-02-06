#ifndef UTILS_H
#define UTILS_H

class vec3f {
public:
	float x;
	float y;
	float z;
};

class Speed {
public:
	float value;
};

class plane4f {
public:
	vec3f normal;
	float d;
};

class DriverActionsState {
public:
	int state;
};

class TyreThermalState {
public:
	float temps[12][3];
	float coreTemp;
	float thermalInput;
	float dynamicPressure;
	float staticPressure;
	float lastSetIMO[3];
	float cpTemperature;
	float lastGrain;
	float lastBlister;
	float mult;
	bool isHot;
};

class mat44f {
public:
	float M11;
	float M12;
	float M13;
	float M14;
	float M21;
	float M22;
	float M23;
	float M24;
	float M31;
	float M32;
	float M33;
	float M34;
	float M41;
	float M42;
	float M43;
	float M44;
};

class SurfaceDef {
public:
	wchar_t wavString[64];
	float wavPitchSpeed;
	void* userPointer;
	float gripMod;
	int sectorID;
	float dirtAdditiveK;
	unsigned int collisionCategory;
	bool isValidTrack;
	float blackFlagTime;
	float sinHeight;
	float sinLength;
	bool isPitlane;
	float damping;
	float granularity;
	float vibrationGain;
	float vibrationLength;
};

class SCarStateAero {
public:
	float CD;
	float CL_Front;
	float CL_Rear;
};

typedef enum CarSetupState {
	UnKnown = 0,
	Legal = 1,
	Illegal = 2
} CarSetupState;

#endif // UTILS_H


