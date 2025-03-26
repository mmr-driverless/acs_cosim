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
void runAssettoCorsa(const std::filesystem::path& acs_dir_path) {
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };

	std::wstring acs_dir = acs_dir_path.wstring();
	std::wstring exe_path = (acs_dir_path / "acs.exe").wstring();
	std::string dll_path = (acs_dir_path / "acs_cosim_dll.dll").string();

	// Iniezione della DLL nel processo
	BOOL result = DetourCreateProcessWithDllEx(
		exe_path.c_str(),
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_DEFAULT_ERROR_MODE,
		NULL,
		acs_dir.c_str(),
		&si,
		&pi,
		dll_path.c_str(),
		NULL
	);

	if (result) {
		std::cerr << "Process created successfully" << std::endl;
	}
	else {
		std::cerr << "Failed to create process. Error: " << GetLastError() << std::endl;
	}
	
	// Attesa della fine del processo
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Cleanup
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}


int actual_main(int argc, char* argv[]) {
	if (argc <= 1) {
		std::cerr << "Usage: " << argv[0] << " ACS_ROOT_DIR" << std::endl;
		return 1;
	}

	std::filesystem::path acs_root_dir(argv[1]);
	// Avvio di Assetto Corsa
	std::cerr << "Launching Assetto Corsa" << std::endl;
	runAssettoCorsa(acs_root_dir / "assettocorsa");
	std::cerr << "Simulator session ended" << std::endl;

	return 0;
}

/**
 * Funzione main
 * 
 * @return Zero
 */
int main(int argc, char* argv[])
{
	int result = actual_main(argc, argv);
	system("pause");
	return result;
}
