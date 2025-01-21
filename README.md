# ACS CoSimulator

![Last update](https://img.shields.io/badge/Last%20update-21%2F01%2F2025-blue)

Simulatore di fisica per settore automotive basato su Assetto Corsa.

## Setup

### Requisiti

- Microsoft Visual Studio 2022
- C++ 20
- C++ desktop app development package
- Windows SDK
- Distribuzione di Assetto Corsa

### Installazione

1. Clona il repository
1. Crea nella radice del progetto una cartella `Dependencies`
1. All'interno di `Dependencies` crea una cartella `Detours`
1. Clona il repository [Detours](https://github.com/microsoft/Detours) all'interno di `Dependencies/Detours`
1. Esegui la build di Detours facendo riferimento alla relativa [documentazione](https://github.com/Microsoft/Detours/blob/main/samples/README.TXT), alla fine del processo dovresti trovare la cartella `Dependencies/Detours/lib.X64`
1. Apri la soluzione `Launcher` in Visual Studio
1. Ora sei pronto ad avviare il simulatore
