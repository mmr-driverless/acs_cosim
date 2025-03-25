#pragma once

#ifdef MMR_SIMULATOR_DLL_EXPORTS
#define MMR_SIMULATOR_API __declspec(dllexport)
#else
#define MMR_SIMULATOR_API __declspec(dllimport)
#endif

extern "C" MMR_SIMULATOR_API void dummy_function();