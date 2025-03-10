// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "MMRSimulatorDll.h"
#include "detours.h"
#include "Car.hpp"
#include <iostream>
#include <acs_cosim/interface/server.hpp>
#include <acs_cosim/interface/constants.hpp>

#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>

#include "Parameters.h"

using namespace acs_cosim::interface::data;
using namespace acs_cosim::interface::server;
using namespace acs_cosim::interface::messages;
using namespace acs_cosim::interface::constants;

AcsServer acs_server("tcp://*:5555");

// Hooked functions
typedef void (*getCarPhysicsStateT)(Car*, CarPhysicsState*);
getCarPhysicsStateT getCarPhysicsState;
typedef void (*carPollControlsT)(Car*, float);
carPollControlsT originalCarPollControlsFunction;

static Parameters params;

void hookedCarPollControls(Car* car, float period) {
	// Call the original function
	if (originalCarPollControlsFunction) {
		originalCarPollControlsFunction(car, period);
	}

	bool advance = true;
	do {
		// Receive action from the Simulator Node
		char buf[MAX_MESSAGE_SIZE];
		acs_server.receive_message(buf);
		MsgBase* msg = (MsgBase*)buf;

		switch (msg->get_type()) {
		case MsgType::Reset:
			std::cerr << "Received RESET message" << std::endl;
			break;
		case MsgType::GetState:
			advance = false;  // Do not step the simulation
			break;
		case MsgType::Control: {
			ControlMsg* msg = (ControlMsg*)buf;
			car->controls = msg->control;
			break;
		}
		default:
			std::cerr << "Unknown message type" << std::endl;
			break;
		}

		// Get and send vehicle state
		CarPhysicsState cps;
		getCarPhysicsState(car, &cps);
		VehicleStateMsg vehicle_state_msg(cps);
		acs_server.send_message(&vehicle_state_msg);
	} while (!advance);
}

void dll_attached(HMODULE hModule) {
	AllocConsole();

	FILE* fDummy;
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONOUT$", "w", stdout);

	std::cerr << "Hello from Assetto Corsa" << std::endl;


	std::wcerr << "ACS_ROOT_DIR = " << params.acs_root_dir() << std::endl;


	// Form ModuleName
	std::string moduleName = (params.acs_root_dir() / "assettocorsa" / "acs.exe").string();
	std::cerr << "Module Name: " << moduleName << std::endl;

	// Ricerca delle funzioni
	void* getCarPhysicsStateAddress = DetourFindFunction(moduleName.c_str(), "Car::getPhysicsState");
	if (getCarPhysicsStateAddress) {
		getCarPhysicsState = (getCarPhysicsStateT)getCarPhysicsStateAddress;
	}
	else {
		std::cerr << "Failed to find function Car::getPhysicsState" << std::endl;
	}

	void* acquireControlsAddress = DetourFindFunction(moduleName.c_str(), "Car::pollControls");
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

