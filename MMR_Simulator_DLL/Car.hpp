#ifndef CAR_HPP
#define CAR_HPP

#include <acs_cosim/interface/CarControls.hpp>
#include <acs_cosim/interface/Utils.hpp>

using namespace acs_cosim::interface::data;

class Car {
public:							// Offset
    char undefined0[0x140];
	CarControls controls;		// 0x140
    char undefined1[0x172];
	float mass;					// 0x1e0
	float ffMult;				// 0x1e4
	vec3f accG;					// 0x1e8
	char undefined2[0x3b94];
	vec3f lastVelocity;			// 0x3d88
	char undefined3[0xfc];
	float speed;				// 0x3e90
	float fuelKG;				// 0x3e94
	bool externalControl;		// 0x3e98
};

#endif // CAR_HPP
