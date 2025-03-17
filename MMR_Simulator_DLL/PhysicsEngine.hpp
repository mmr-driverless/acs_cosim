#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP

struct PhysicsEngine {
	char undefined0[0x20];
	double physicsTime;	// 0x20
	char undefined1[0xd0];
	double gameTime;	// 0xf8
};

#endif // !PHYSICS_ENGINE_HPP
