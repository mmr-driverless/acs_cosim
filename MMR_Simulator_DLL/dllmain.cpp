// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "MMRSimulatorDll.h"
#include "detours.h"
#include "Car.h"
#include "CarPhysicsState.h"
#include <iostream>

#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>

const char* moduleName = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\assettocorsa\\acs.exe";
const char* outputFile = ".\\acs_data.csv";

// Hooked functions
typedef void (*car_step_t)(Car*, float);
car_step_t originalCarStepFunction;
typedef void (*getCarPhysicsState_t)(Car*, CarPhysicsState*);
getCarPhysicsState_t originalGetCarPhysicsStateFunction;

void hookedCarStepFunction(Car* car, float param_1) {
	try {
		std::ofstream file(outputFile, std::ios::app);
		if (!file.is_open()) {
			throw std::ios_base::failure("Failed to open the file for writing.");
		}

		// Get current time
		auto now = std::chrono::system_clock::now();

		CarPhysicsState cps;
		originalGetCarPhysicsStateFunction(car, &cps);

		file
			<< now.time_since_epoch().count() << ","
			<< cps.engineRPM << ","
			<< cps.limiterRPM << ","
			<< cps.steer << ","
			<< cps.gas << ","
			<< cps.brake << ","
			<< cps.clutch << ","
			<< cps.gear << ","
			<< cps.speed.value << ","
			<< cps.velocity.x << ","
			<< cps.velocity.y << ","
			<< cps.velocity.z << ","
			<< cps.localVelocity.x << ","
			<< cps.localVelocity.y << ","
			<< cps.localVelocity.z << ","
			<< cps.localAngularVelocity.x << ","
			<< cps.localAngularVelocity.y << ","
			<< cps.localAngularVelocity.z << ","
			<< cps.angularVelocity.x << ","
			<< cps.angularVelocity.y << ","
			<< cps.angularVelocity.z << ","
			<< cps.accG.x << ","
			<< cps.accG.y << ","
			<< cps.accG.z << ","
			<< cps.worldMatrix.M11 << ","
			<< cps.worldMatrix.M12 << ","
			<< cps.worldMatrix.M13 << ","
			<< cps.worldMatrix.M14 << ","
			<< cps.worldMatrix.M21 << ","
			<< cps.worldMatrix.M22 << ","
			<< cps.worldMatrix.M23 << ","
			<< cps.worldMatrix.M24 << ","
			<< cps.worldMatrix.M31 << ","
			<< cps.worldMatrix.M32 << ","
			<< cps.worldMatrix.M33 << ","
			<< cps.worldMatrix.M34 << ","
			<< cps.worldMatrix.M41 << ","
			<< cps.worldMatrix.M42 << ","
			<< cps.worldMatrix.M43 << ","
			<< cps.worldMatrix.M44
			<< std::endl;
		file.close();
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	// Chiamata alla funzione originale
	if (originalCarStepFunction) {
		originalCarStepFunction(car, param_1);
	}
}

void dll_attached(HMODULE hModule) {
	// Find the address of the PhysicsEngine::run function
	void* carStepAddress = DetourFindFunction(moduleName, "Car::step");

	// Hook the function using Detours.
	if (carStepAddress) {
		originalCarStepFunction = (car_step_t)carStepAddress;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)originalCarStepFunction, hookedCarStepFunction);
		DetourTransactionCommit();
	}
	else {
		std::cerr << "Failed to find function Car::step" << std::endl;
	}

	void* getCarPhysicsStateAddress = DetourFindFunction(moduleName, "Car::getPhysicsState");
	if (getCarPhysicsStateAddress) {
		originalGetCarPhysicsStateFunction = (getCarPhysicsState_t)getCarPhysicsStateAddress;
	}
	else {
		std::cerr << "Failed to find function Car::getPhysicsState" << std::endl;
	}
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		dll_attached(hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

