#include <iostream>
#include <windows.h>
#include <detours.h>
#include <filesystem>
#include <stdexcept>
#include <shlobj.h> // For SHGetKnownFolderPath

namespace fs = std::filesystem;

// Function to get the path to the Documents folder using Windows API
fs::path getEnv(LPCSTR variableName) {
	char envVariableValue[MAX_PATH];
	DWORD length = GetEnvironmentVariableA(variableName, envVariableValue, MAX_PATH);

	if (length == 0 || length >= MAX_PATH) {
		throw std::runtime_error("Failed to retrieve environment variable.");
	}

	return envVariableValue;
}

fs::path getDocumentsFolderPath() {
	PWSTR path = nullptr;
	// Retrieve the path to the Documents folder
	if (SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path) != S_OK) {
		throw std::runtime_error("Failed to retrieve Documents folder path.");
	}

	// Convert wide string to std::filesystem::path
	fs::path documentsPath = path;
	CoTaskMemFree(path); // Free memory allocated by SHGetKnownFolderPath
	return documentsPath;
}

int loadACSConfig() {
	// Check if the source directory exists
	fs::path sourcePath = "./static/cfg/";
	if (!fs::exists(sourcePath) || !fs::is_directory(sourcePath)) {
		throw std::runtime_error("Error: Source directory does not exist or is not a directory.");
	}

	// Check if the destination directory exists
	fs::path destinationPath = getDocumentsFolderPath() / "Assetto Corsa" / "cfg";
	if (!fs::exists(destinationPath)) {
		throw std::runtime_error("Error: Destination folder does not exist.");
	}

	// Iterate over the files and directories in the source directory
	for (const auto& entry : fs::recursive_directory_iterator(sourcePath)) {
		const fs::path relativePath = fs::relative(entry.path(), sourcePath);
		const fs::path fileDestinationPath = destinationPath / relativePath;

		if (fs::is_directory(entry)) {
			// Create subdirectories in the target directory
			fs::create_directories(fileDestinationPath);
		}
		else if (fs::is_regular_file(entry)) {
			// Copy files to the target directory
			fs::copy_file(entry, fileDestinationPath, fs::copy_options::overwrite_existing);
		}
	}
	std::cout << "All configuration files copied successfully to " << destinationPath << ".\n";
}

void runAssettoCorsa(LPCWSTR assettoCorsaDir, LPCSTR dll) {
	std::wstring assettoCorsaDirWString(assettoCorsaDir);
	std::wstring targetExeWString = assettoCorsaDirWString + L"/acs.exe";
	LPCWSTR targetExe = targetExeWString.c_str();

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };

	// Inject DLL file into the process
	BOOL result = DetourCreateProcessWithDlls(
		targetExe,
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_DEFAULT_ERROR_MODE,
		NULL,
		assettoCorsaDir,
		&si,
		&pi,
		0,
		&dll,
		NULL
	);

	if (result) {
		std::cout << "Process created successfully" << std::endl;
	}
	else {
		std::cout << "Failed to create process. Error: " << GetLastError() << std::endl;
	}
	
	// Wait for the process to exit.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Cleanup.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

int main()
{
	// Config
	const LPCWSTR assettoCorsaDir = L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\assettocorsa";
	const LPCSTR dll = "";
	const BOOL useACSConfig = TRUE;

	// Load settings for Assetto Corsa
	if (useACSConfig) {
		loadACSConfig();
	}

	// Run Assetto Corsa
	std::cout << "Launching Assetto Corsa" << std::endl;
	runAssettoCorsa(assettoCorsaDir, dll);
	std::cout << "Simulator session ended" << std::endl;
}
