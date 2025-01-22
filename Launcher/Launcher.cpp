#include <iostream>
#include <windows.h>
#include <detours.h>
#include <filesystem>
#include <stdexcept>
#include <shlobj.h> // For SHGetKnownFolderPath

namespace fs = std::filesystem;

/**
 * Funzione che restituisce il percorso della cartella Documenti dell'utente attuale.
 * 
 * @return Percorso della cartella Documenti
 */
fs::path getDocumentsFolderPath() {
	PWSTR path = nullptr;
	if (SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path) != S_OK) {
		throw std::runtime_error("Failed to retrieve Documents folder path.");
	}

	fs::path documentsPath = path;
	CoTaskMemFree(path); // Liberazione della memoria allocata da SHGetKnownFolderPath
	return documentsPath;
}

/**
 * Carica una configurazione preipostata da far utilizzare ad Assetto Corsa.
 * 
 * Per farlo copia i file contenenti le impostazioni dentro la cartella di Assetto Corsa
 * che si trova nella cartella Documenti dell'utente.
 */
void loadACSConfig() {
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

/**
 * Funzione che avvia la simulazione in Assetto Corsa, iniettando la DLL con il codice arbitrario
 * 
 * @param assettoCorsaDir Percorso della cartella di installazione di Assetto Corsa
 * @param dll Percorso della DLL da iniettare
 */
void runAssettoCorsa(LPCWSTR assettoCorsaDir, LPCSTR dll) {
	std::wstring assettoCorsaDirWString(assettoCorsaDir);
	std::wstring targetExeWString = assettoCorsaDirWString + L"/acs.exe";
	LPCWSTR targetExe = targetExeWString.c_str();

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };

	// Iniezione della DLL nel processo
	BOOL result = DetourCreateProcessWithDllEx(
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
		dll,
		NULL
	);

	if (result) {
		std::cout << "Process created successfully" << std::endl;
	}
	else {
		std::cout << "Failed to create process. Error: " << GetLastError() << std::endl;
	}
	
	// Attesa della fine del processo
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Cleanup
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

/**
 * Funzione main
 * 
 * @return Zero
 */
int main()
{
	// Variabili di configurazione
	const LPCWSTR assettoCorsaDir = L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\assettocorsa";
	const std::string dllAbsolutePath = fs::absolute("..\\MMR_Simulator_DLL\\x64\\Debug\\MMR_Simulator_DLL.dll").string();
	const LPCSTR dll = dllAbsolutePath.c_str();
	const BOOL useACSConfig = TRUE;

	// Caricamento delle impostazioni per Assetto Corsa
	if (useACSConfig) {
		loadACSConfig();
	}

	// Avvio di Assetto Corsa
	std::cout << "Launching Assetto Corsa" << std::endl;
	runAssettoCorsa(assettoCorsaDir, dll);
	std::cout << "Simulator session ended" << std::endl;

	return 0;
}
