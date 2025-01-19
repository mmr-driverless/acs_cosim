#include <iostream>
#include <windows.h>
#include <detours.h>

int main()
{
	LPCWSTR targetExe = L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\assettocorsa\\acs.exe";
	LPCWSTR workingDir = L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\assettocorsa";
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };
	LPCSTR dll = "";

	std::cout << "Launching process" << std::endl;
	// Inject DLL file into the process
	BOOL result = DetourCreateProcessWithDlls(
		targetExe,
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_DEFAULT_ERROR_MODE,
		NULL,
		workingDir,
		&si,
		&pi,
		0,
		&dll,
		NULL
	);

	// Wait for the process to exit.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Cleanup.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if (result) {
		std::cout << "Process created successfully" << std::endl;
	}
	else {
		std::cout << "Failed to create process. Error: " << GetLastError() << std::endl;
	}

}
