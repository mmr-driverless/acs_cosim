// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "MMRSimulatorDll.h"
#include "detours.h"
#include "Car.h"
#include <iostream>

#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>

const char* moduleName = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\assettocorsa\\acs.exe";
const char* outputFile = ".\\acs_data.txt";

typedef void (*car_step_t)(Car*, float);
car_step_t originalCarStepFunction;

void hookedCarStepFunction(Car* car, float param_1) {
	try {
		// Get current time
		auto now = std::chrono::system_clock::now();
		auto now_time_t = std::chrono::system_clock::to_time_t(now);

		// Convert to local time using localtime_s
		std::tm local_tm;
		if (localtime_s(&local_tm, &now_time_t) != 0) {
			throw std::runtime_error("Failed to convert time to local time.");
		}

		// Open file in the current directory
		std::ofstream file(outputFile, std::ios::app);
		if (!file.is_open()) {
			throw std::ios_base::failure("Failed to open the file for writing.");
		}

		CarControls controls = car->controls;
		// Write timestamp to the file in a human-readable format
		file << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << " - " << controls.steer << " - " << controls.gas << std::endl;
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

		std::cout << ">>> Successfully hooked Car::step" << std::endl;
	}
	else {
		std::cerr << ">>> Failed to find function Car::step in module " << moduleName << std::endl;
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

