// dllmain.cpp : Defines the entry point for the DLL application.

#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>

#include <windows.h>
#include <detours.h>

#include <acs_cosim/interface/server.hpp>
#include <acs_cosim/interface/constants.hpp>

#include "Car.hpp"
#include "CarAvatar.hpp"
#include "UDPCommandListener.hpp"
#include "PhysicsDriveThread.hpp"
#include "Parameters.h"

using namespace acs_cosim::interface::data;
using namespace acs_cosim::interface::server;
using namespace acs_cosim::interface::messages;
using namespace acs_cosim::interface::constants;

AcsServer acs_server("tcp://*:5555");
UDPCommandListener* commandListener = nullptr;
PhysicsDriveThread* physicsDriveThread = nullptr;

// Hooked functions
typedef void (*getCarPhysicsStateT)(Car*, CarPhysicsState*);
getCarPhysicsStateT getCarPhysicsState;

typedef void (*carPollControlsT)(Car*, float);
carPollControlsT originalCarPollControlsFunction;

typedef void (*commandListenerUpdateT)(UDPCommandListener*, float);
commandListenerUpdateT originalCommandListenerUpdateFunction;

typedef void (*simStartGameT)(Sim*);
simStartGameT originalSimStartGameFunction;

typedef CarAvatar* (*simGetCarT)(void*, int);
simGetCarT originalSimGetCarFunction;

typedef void (*goToSpawnPositionT)(void*, std::wstring);
goToSpawnPositionT originalGoToSpawnPosition;

typedef void (*physicsDriveThreadRunT)(PhysicsDriveThread*);
physicsDriveThreadRunT originalPhysicsDriveThreadRun;

static Parameters params;

bool firstStep = true;
std::chrono::milliseconds simulationTime(0);

void sendSimulationStateMsg(Car* car) {
	CarPhysicsState cps;
	getCarPhysicsState(car, &cps);
	SimulationState simulationState(simulationTime, cps);
	SimulationStateMsg simulationStateMsg(simulationState);
	acs_server.send_message(&simulationStateMsg);
}

/**
 * The only purpose of this function is to find the address of PhysicsDriveThread
 */
void hookedPhysicsDriveThreadRun(PhysicsDriveThread* pdt) {
	if (physicsDriveThread == nullptr) {
		physicsDriveThread = pdt;
	}

	if (originalPhysicsDriveThreadRun) {
		originalPhysicsDriveThreadRun(pdt);
	}
}

/**
 * This function gets the address of the UDPCommandListener object
 * in order to control the status of the simulation.
 * Then starts the simulation.
 */
void hookedCommandListenerUpdateFunction(UDPCommandListener* localCommandListener, float period) {
	if (commandListener == nullptr) {
		commandListener = localCommandListener;

		// Start the simulation
		if (originalSimStartGameFunction) {
			originalSimStartGameFunction(commandListener->sim);
		}
	}

	if (originalCommandListenerUpdateFunction) {
		originalCommandListenerUpdateFunction(localCommandListener, period);
	}
}

void hookedCarPollControls(Car* car, float period) {
	if (!firstStep) {
		// Vehicle state is sent here because the step needs to be concluded before sending the state to AcsClient.
		// To do so, this function (hookedCarPollControls) needs to return. When this function is called again
		// during the next simulation step, it sends the updated SimulationStateMsg to the client.
		sendSimulationStateMsg(car);
		
		// simulationTime is incremented here because we want to increment whenever this function is called
		// exept for the first step
		simulationTime += std::chrono::milliseconds(3);
	}
	else {
		firstStep = false;
	}

	// Call the original pollControls function, in order to ovverride its result
	if (originalCarPollControlsFunction) {
		originalCarPollControlsFunction(car, period);
	}

	bool advance;
	do {
		advance = true;

		// Receive action from the Simulator Node
		char buf[MAX_MESSAGE_SIZE];
		acs_server.receive_message(buf);
		MsgBase* msg = (MsgBase*)buf;

		switch (msg->get_type()) {
		case MsgType::Reset:
			if (originalSimGetCarFunction && originalGoToSpawnPosition) {
				CarAvatar* carAvatar = originalSimGetCarFunction(commandListener->sim, 0);
				originalGoToSpawnPosition(carAvatar, L"PIT");
			}
			break;
		case MsgType::GetState:
			advance = false;  // Do not step the simulation
			sendSimulationStateMsg(car);
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
	} while (!advance);

	if (physicsDriveThread) {
		physicsDriveThread->currentTime = car->ksPhysics->gameTime;
	}
}

void dll_attached(HMODULE hModule) {
	AllocConsole();

	FILE* fDummy;
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONOUT$", "w", stdout);

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

	void* commandListenerUpdate = DetourFindFunction(moduleName.c_str(), "UDPCommandListener::update");
	if (commandListenerUpdate) {
		originalCommandListenerUpdateFunction = (commandListenerUpdateT)commandListenerUpdate;
	}
	else {
		std::cerr << "Failed to find function UDPCommandListener::update" << std::endl;
	}

	void* simStartGame = DetourFindFunction(moduleName.c_str(), "Sim::startGame");
	if (simStartGame) {
		originalSimStartGameFunction = (simStartGameT)simStartGame;
	}
	else {
		std::cerr << "Failed to find function Sim::startGame" << std::endl;
	}

	void* simGetCar = DetourFindFunction(moduleName.c_str(), "Sim::getCar");
	if (simGetCar) {
		originalSimGetCarFunction = (simGetCarT)simGetCar;
	}
	else {
		std::cerr << "Failed to find function Sim::getCar" << std::endl;
	}

	void* goToSpawnPosition = DetourFindFunction(moduleName.c_str(), "CarAvatar::goToSpawnPosition");
	if (goToSpawnPosition) {
		originalGoToSpawnPosition = (goToSpawnPositionT)goToSpawnPosition;
	}
	else {
		std::cerr << "Failed to find function CarAvatar::goToSpawnPosition" << std::endl;
	}

	void* physicsDriveThreadRun = DetourFindFunction(moduleName.c_str(), "PhysicsDriveThread::run");
	if (physicsDriveThreadRun) {
		originalPhysicsDriveThreadRun = (physicsDriveThreadRunT)physicsDriveThreadRun;
	}
	else {
		std::cerr << "Failed to find function PhysicsDriveThread::run" << std::endl;
	}

	// Hooking delle funzioni
	if (acquireControlsAddress) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)originalCarPollControlsFunction, hookedCarPollControls);
		DetourAttach(&(PVOID&)originalCommandListenerUpdateFunction, hookedCommandListenerUpdateFunction);
		DetourAttach(&(PVOID&)originalPhysicsDriveThreadRun, hookedPhysicsDriveThreadRun);
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

