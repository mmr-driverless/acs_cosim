# ACS CoSimulator

![Last update](https://img.shields.io/badge/Last%20update-06%2F03%2F2025-blue)

Physics simulator based on Assetto Corsa.

## Setup

This project is made of two Microsoft Visual Studio solutions. B**efore opening the solutions** be sure to create your own `.props` file by copying `.props.example`, and setting you own values for the variables:

- **ASSETTO_CORSA_PARENT_DIR**: it should be a directory containing a folder named `assettocorsa`

### Tools

- Windows
- Microsoft Visual Studio 2022
- C++ 20
- C++ desktop app development package
- Windows SDK
- Assetto Corsa

### Dependencies

- [Detours](https://github.com/microsoft/Detours)
- [ZeroMQ](https://github.com/zeromq/libzmq) (libzmq for C)
- [acs_cosim_interface](https://github.com/mmr-driverless/acs_cosim_interface) (Already included in a submodule)

### Installing

**Detours:**

1. Clone the repository
1. Inside of `Dependencies` create a folder `Detours`
1. Clone the repository [Detours](https://github.com/microsoft/Detours) inside of `Dependencies/Detours`
1. Build Detours referring to the [documentation](https://github.com/Microsoft/Detours/blob/main/samples/README.TXT), at the end of the process you should be able to find the folder `Dependencies/Detours/lib.X64`

**ZeroMQ:**

1. Clone [ZeroMQ](https://github.com/zeromq/libzmq) in the `Dependencies` folder
1. Build however you want (vcpkg recommended, from the zmqlib README)

**Acs_cosim_interface**

1. Open *x64 Native Tools Command Prompt for VS* with administrator privileges
1. Cd to the `acs_cosim_interface` folder and create an `out` folder
1. Get inside the `out` folder and run `cmake ..`
1. When that has finished run `msbuild INSTALL.vcproj`

## Running

1. Open the `MMR_Simulator_DLL` solution in Visual Studio
1. Build the solution (Menu -> Build -> Build Solution)
1. Open the `Launcher` solution in Visual Studio
1. Simply start the program by pressing the play button or `Ctrl-F5`

Now you should see two terminals being opened and then Assetto Corsa launches, then gets stuck waiting for commands from the Simulator ROS Node.

## Troubleshooting

If building `MMR_Simulator_DLL` fails, and the message refers to ZeroMQ, you probably need to change the path of where built ZeroMQ files are:
  1. go to `acs_cosim_interface` folder
  1. open `CMakeLists.txt` in an editor
  1. change the path of ZeroMQ

If starting the program raises an error requiring for some dll, find the dll path in the build directories and add it to the PATH environment variable.
