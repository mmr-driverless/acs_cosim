// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "MMRSimulatorDll.h"
#include "detours.h"
#include "Car.h"
#include "CarControls.h"
#include "CarPhysicsState.h"
#include <iostream>

#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>

const char* moduleName = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\assettocorsa\\acs.exe";
const char* outputFile = ".\\acs_data.csv";

// Hooked functions
typedef void (*getCarPhysicsStateT)(Car*, CarPhysicsState*);
getCarPhysicsStateT getCarPhysicsState;
typedef void (*carPollControlsT)(Car*, float);
carPollControlsT originalCarPollControlsFunction;

bool firstStep = true;
int steerCounter = 0;
float steer = -1;
int gasCounter = 0;

void hookedCarPollControls(Car* car, float period) {
	// Call the original function
	if (originalCarPollControlsFunction) {
		originalCarPollControlsFunction(car, period);
	}

	// Take control of the car
	if (firstStep) {
		car->controls.steer = -1;

		car->controls.clutch = 1;
		car->controls.gearUp = true;
		firstStep = false;
		return;
	}
	else {
		car->controls.clutch = 0;
		car->controls.gearUp = false;
	}

	if (gasCounter > 3000) {
		car->controls.gas = 1;
	}

	steerCounter++;
	car->controls.steer = steer;
	if (steerCounter > 500) {
		steer = -steer;
		steerCounter = 0;
	}

	gasCounter++;

	try {
		std::ofstream file(outputFile, std::ios::app);
		if (!file.is_open()) {
			throw std::ios_base::failure("Failed to open the file for writing.");
		}

		// Get current time and CarPhysicsState
		auto now = std::chrono::system_clock::now();

		CarPhysicsState cps;
		getCarPhysicsState(car, &cps);

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
}

void dll_attached(HMODULE hModule) {
	// Ricerca delle funzioni
	void* getCarPhysicsStateAddress = DetourFindFunction(moduleName, "Car::getPhysicsState");
	if (getCarPhysicsStateAddress) {
		getCarPhysicsState = (getCarPhysicsStateT)getCarPhysicsStateAddress;
	}
	else {
		std::cerr << "Failed to find function Car::getPhysicsState" << std::endl;
	}

	void* acquireControlsAddress = DetourFindFunction(moduleName, "Car::pollControls");
	if (acquireControlsAddress) {
		originalCarPollControlsFunction = (carPollControlsT)acquireControlsAddress;
	}
	else {
		std::cerr << "Failed to find function Car::pollControls" << std::endl;
	}

	// Hooking delle funzioni
	if (acquireControlsAddress) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)originalCarPollControlsFunction, hookedCarPollControls);
		DetourTransactionCommit();
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

