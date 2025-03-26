# ACS CoSimulator

![Last update](https://img.shields.io/badge/Last%20update-06%2F03%2F2025-blue)

Physics simulator based on Assetto Corsa.

## Setup

Remember to clone using `--recurse-submodules` (or `--recursive`) to correctly initialize the submodules.
Alternatively, if you already cloned the repository, you can initialize them by running `git submodule update --init --recursive`.

Open the repository folder with Visual Studio.
Configure the CMake cache either via "Project" in the toolbar or via context menu on the root CMakeLists.txt.

You can build the project via "Build" in the toolbar and clicking "Build All".
You can install the project via "Build" in the toolbar and clicking "Install". This will create the release which you can zip up and upload to GitHub.

### Tools

- Windows
- Microsoft Visual Studio 2022
- C++ 20
- C++ desktop app development package
- Windows SDK
- Assetto Corsa

### Dependencies

- [Detours](https://github.com/microsoft/Detours) (Already included in a submodule)
- [acs_cosim_interface](https://github.com/mmr-driverless/acs_cosim_interface) (Already included in a submodule)
