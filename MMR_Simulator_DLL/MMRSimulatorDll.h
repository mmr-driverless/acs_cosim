#pragma once

#ifdef MYDLL_EXPORTS
#define MYDLL __declspec(dllexport)
#else
#define MYDLL __declspec(dllimport)
#endif

extern "C" MYDLL void dll_attached();