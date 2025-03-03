// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "MMRSimulatorDll.h"
#include "detours.h"
#include "Car.h"
#include "CarControls.h"
#include "CarPhysicsState.h"
#include <iostream>
#include <acs_cosim/interface/server.hpp>
#include <acs_cosim/interface/constants.hpp>

#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>

using namespace acs_cosim::interface::data;
using namespace acs_cosim::interface::server;
using namespace acs_cosim::interface::messages;
using namespace acs_cosim::interface::constants;

AcsServer acs_server("tcp://*:5555");

const char* moduleName = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\assettocorsa\\acs.exe";

// Hooked functions
typedef void (*getCarPhysicsStateT)(Car*, CarPhysicsState*);
getCarPhysicsStateT getCarPhysicsState;
typedef void (*carPollControlsT)(Car*, float);
carPollControlsT originalCarPollControlsFunction;

void hookedCarPollControls(Car* car, float period) {
	// Call the original function
	if (originalCarPollControlsFunction) {
		originalCarPollControlsFunction(car, period);
	}

	// Receive action from the Simulator Node
	char buf[MAX_MESSAGE_SIZE];
	acs_server.receive_message(buf);

	MsgBase* msg = (MsgBase*)buf;

	switch (msg->get_type()) {
	case MsgType::Reset:
		std::cerr << "Received RESET message" << std::endl;
		break;
	case MsgType::GetState:
		std::cerr << "Received GET_STATE message" << std::endl;
		break;
	case MsgType::Control: {
		std::cerr << "Received CONTROL message" << std::endl;
		ControlMsg* msg = (ControlMsg*)buf;
		std::cerr << "Torque: " << msg->control.torque << std::endl;
		std::cerr << "Steering angle: " << msg->control.steering_angle
			<< std::endl;
		std::cerr << "Cycles to skip: " << msg->cycles_to_skip << std::endl;
		break;
	}
	default:
		std::cerr << "Unknown message type" << std::endl;
		break;
	}

	// Get and send vehicle state
	CarPhysicsState cps;
	getCarPhysicsState(car, &cps);

	VehicleState vehicle_state;
	vehicle_state.angular_velocity.x = cps.angularVelocity.x;
	vehicle_state.angular_velocity.y = cps.angularVelocity.y;
	vehicle_state.angular_velocity.z = cps.angularVelocity.z;
	vehicle_state.linear_velocity.x = cps.velocity.x;
	vehicle_state.linear_velocity.y = cps.velocity.y;
	vehicle_state.linear_velocity.z = cps.velocity.z;
	vehicle_state.rotation.m[0][0] = cps.worldMatrix.M11;
	vehicle_state.rotation.m[0][1] = cps.worldMatrix.M12;
	vehicle_state.rotation.m[0][2] = cps.worldMatrix.M13;
	vehicle_state.rotation.m[1][0] = cps.worldMatrix.M21;
	vehicle_state.rotation.m[1][1] = cps.worldMatrix.M22;
	vehicle_state.rotation.m[1][2] = cps.worldMatrix.M23;
	vehicle_state.rotation.m[2][0] = cps.worldMatrix.M31;
	vehicle_state.rotation.m[2][1] = cps.worldMatrix.M32;
	vehicle_state.rotation.m[2][2] = cps.worldMatrix.M33;
	vehicle_state.position.x = cps.worldMatrix.M41;
	vehicle_state.position.y = cps.worldMatrix.M42;
	vehicle_state.position.z = cps.worldMatrix.M43;
	VehicleStateMsg vehicle_state_msg(vehicle_state);
	acs_server.send_message(&vehicle_state_msg);
}

void dll_attached(HMODULE hModule) {
	AllocConsole();

	FILE* fDummy;
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONOUT$", "w", stdout);

	std::cerr << "Hello from Assetto Corsa" << std::endl;

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

