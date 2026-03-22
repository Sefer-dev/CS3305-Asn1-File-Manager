# File Manager — wxWidgets Desktop Application

A native Windows file manager built with **C++17** and **wxWidgets**, featuring directory browsing, file operations (create, open, rename, delete), clipboard functionality (copy, cut, paste), and keyboard shortcuts. Built for CS 3307 — Software Engineering at Western University.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Screenshots](#screenshots)
- [Getting Started](#getting-started)
- [Project Structure](#project-structure)
- [Usage](#usage)
- [Technologies](#technologies)

## Overview

This application is a lightweight GUI file manager that provides core file system operations through an intuitive interface. It uses the C++17 `<filesystem>` library for all file operations and wxWidgets for the cross-platform GUI framework. The main window displays the contents of the current directory in a detailed list view with columns for name, type, size, and date modified.

## Features

- **Directory Navigation** — Browse directories by double-clicking folders, clicking `..` to go up, or typing a path directly into the address bar
- **Create** — Create new directories via `File > New` or `Ctrl+N`
- **Open** — Open files with their default system application or navigate into directories (`Ctrl+O`)
- **Rename** — Rename files and directories with duplicate-name detection (`Ctrl+E`)
- **Delete** — Delete files or recursively delete directories with a confirmation dialog (`Del`)
- **Copy / Cut / Paste** — Clipboard-based file operations with overwrite confirmation (`Ctrl+C`, `Ctrl+X`, `Ctrl+V`)
- **Refresh** — Reload the current directory listing (`F5`)
- **Sorted Listing** — Directories are listed first, then files, both in alphabetical order
- **Human-Readable Sizes** — File sizes displayed as KB, MB, or GB
- **Status Bar** — Displays item count and operation feedback

## Screenshots

*Add screenshots of the application here.*

## Getting Started

### Prerequisites

- **Visual Studio 2022** (or compatible MSVC toolchain with C++17 support)
- **wxWidgets 3.x** — prebuilt or compiled from source

### Setting Up wxWidgets

1. Download wxWidgets from [wxwidgets.org/downloads](https://www.wxwidgets.org/downloads/).
2. Build the library or download prebuilt binaries for MSVC.
3. Place the wxWidgets folder somewhere accessible (e.g., `C:\libraries\wx`).

### Configuring the Project

The `.vcxproj` expects wxWidgets at a specific path. You will need to update the include and library paths to match your local setup:

1. Open the solution in Visual Studio.
2. Right-click the project → **Properties**.
3. Under **C/C++ → General → Additional Include Directories**, set:
   ```
   <your-wx-path>\include
   <your-wx-path>\include\msvc
   ```
4. Under **Linker → General → Additional Library Directories**, set:
   ```
   <your-wx-path>\lib\vc_lib
   ```

> Replace `<your-wx-path>` with the actual path to your wxWidgets installation.

### Building

1. Open `File Manager.vcxproj` in Visual Studio.
2. Select your build configuration (Debug/Release) and platform (Win32/x64).
3. Build the solution (`Ctrl+Shift+B`).
4. Run with `F5` or find the executable in the `Debug/` or `Release/` folder.

## Project Structure

```
├── .gitignore
├── README.md
├── File Manager.vcxproj            # Visual Studio project file
├── File Manager.vcxproj.filters    # VS source file grouping
├── FileManagerApp.h                # Application class declaration
├── FileManagerApp.cpp              # Application entry point (OnInit)
├── FileManagerFrame.h              # Main window class declaration
└── FileManagerFrame.cpp            # Main window implementation (all features)
```

| File | Purpose |
|------|---------|
| `FileManagerApp` | Initializes the wxWidgets application and creates the main window |
| `FileManagerFrame` | Contains all UI setup, event handling, and file system operations |

## Usage

### Keyboard Shortcuts

| Shortcut   | Action                |
|------------|-----------------------|
| `Ctrl+N`   | New directory         |
| `Ctrl+O`   | Open file / directory |
| `Ctrl+E`   | Rename                |
| `Del`      | Delete                |
| `Ctrl+C`   | Copy                  |
| `Ctrl+X`   | Cut                   |
| `Ctrl+V`   | Paste                 |
| `F5`       | Refresh               |
| `Alt+F4`   | Exit                  |
| `Enter`    | Navigate to typed path|

### Menu Bar

- **File** — New, Open, Rename, Delete, Exit
- **Edit** — Copy, Cut, Paste, Refresh

## Technologies

- **Language:** C++17
- **GUI Framework:** [wxWidgets](https://www.wxwidgets.org/)
- **File System:** C++17 `<filesystem>` library
- **Build System:** Visual Studio 2022 / MSVC
- **Platform:** Windows

## Author

Melvin Roger — University of Western Ontario, CS 3307

## License

This project was completed as a course assignment. Please reference responsibly.
