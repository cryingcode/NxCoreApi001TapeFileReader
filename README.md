# NxCore Tape File Reader and Status Message

[![C++17](https://img.shields.io/badge/C++-17-blue?logo=c%2B%2B&logoColor=white)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/CMake-3.25+-green?logo=cmake&logoColor=white)](https://cmake.org)
[![Ubuntu 24.04](https://img.shields.io/badge/Ubuntu-24.04-orange?logo=ubuntu&logoColor=white)](https://ubuntu.com)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
<!-- [![CI - Build & Test](https://github.com/cryingcode/NxCoreApi001TapeFileReader/actions/workflows/ci-build.yml/badge.svg)](https://github.com/cryingcode/NxCoreApi001TapeFileReader/actions) -->

NxCore historical tapefile reader (.nx2/.nx3): Linux-based C++17 code built using modern CMake and demonstrating basic callback usage by processing the Status Message.

**Status:** Tested on Ubuntu 24.04 LTS with libnx.so Linux x64 (latest version as of early 2026).

## Quick Start (Ubuntu 24.04)
From the main project directory (`NxCoreApiTapeFileReader/`) the quick start uses a convenience script:
```bash
# 1. Place libnx.so in ./extern/nxcore/libnx.so
# 2. Build & install from the main project directory
./qb.sh
# 3. Run on a test tape file (libnx.so name works after ldconfig)
nxcore-tape-reader libnx.so /path/to/your.tape.nx2
```

Most NxCore examples are Windows-oriented and use very old build systems. This project shows how to use modern CMake (3.25+), out-of-source builds, proper rpath handling, and clean C++17 callback wrappers on Linux — while staying as close as possible to the original Sample1 logic.

## Goals
1. Modern CMake + install + find_package
2. Correct Linux .so loading & rpath
3. Faithful callback usage from Sample1
4. Robust error handling
5. Reproducible / container-friendly

## Demonstrated API Usage
1. Initializing and calling `NxCore_ProcessTape`
2. Defining and registering a user callback function
3. Processing `NxCORESTATUS` messages and basic event handling

## Prerequisites

#### Critical Prerequisite — NxCore Linux library

You **must** obtain `libnx.so` (x64 Linux version) directly from Nanex:

[https://www.nxcoredata.com/nxcore-downloads](https://www.nxcoredata.com/nxcore-downloads)  
(requires free registration / trial — no public direct link)

Place it in: `extern/nxcore/libnx.so`

#### Required Tools & Versions (Ubuntu 24.04 LTS example)
| Package              | Recommended Version     | Notes                          | Install Command |
|----------------------|-------------------------|--------------------------------|------|
| C++ compiler         | g++ 13.2+               | C++17 required                 | sudo apt install build-essential |
| CMake                | 3.25+                   | Modern feature set used        | sudo apt install cmake |
| Ninja                | 1.10+                   | Optional (CMake can use Make)  | sudo apt install ninja-build |
| NxCore API library   | `libnx.so` v3.2.18+     | Nanex / NxCore Market Data     | see link above |

#### Install build essentials on Ubuntu:
```bash
sudo apt update
sudo apt install build-essential cmake ninja-build
```
## Project Structure
Obtain `libnx.so` and the NxCore API headers (NxCoreAPI.h, etc.) from: https://www.nxcoredata.com/nxcore-downloads.  Place the proprietary headers under the `src/` directory and the library object file in the `extern/nxcore/` directory as shown in the project structure.  CMake working directories (build/, bin/, etc.), the `libnx.so` library object file, and the proprietary headers are excluded from version control via `.gitignore`.  Legacy naming in subdirectories of `srcinc/` and `srclib/` preserves fidelity to the original Nanex sample for easy comparison/diffing.

```text
README.md
LICENSE.md
NxCoreApiTapeFileReader/
├── CMakeLists.txt
├── src/
│   ├── executableUtils.cpp
│   ├── main.cpp
│   ├── nxcaExceptions.cpp
│   ├── nxcore_global.cpp
│   ├── nxcoreCallback.cpp
│   ├── processNxCoreStatusMessage.cpp
│   ├── srcinc/
│   │   ├── executableUtils.hpp
│   │   ├── nxcaExceptions.hpp
│   │   ├── nxcoreCallback.hpp
│   │   ├── processNxCoreStatusMessage.hpp
│   │   └── nxcore                       # Legacy header stubs for compatibility
│   │       ├── StdAfx.h                 # MS Windows header
│   │       └── targetver.h              # MS Windows header
│   └── srclib/
│       └── nxcore                       # Proprietary API headers & C++ wrapper
│           ├── NxCoreAPI.h
│           └── NxCoreAPI_Wrapper_C++.h
├── include/                             # Public project API headers (if any)
├── extern/                              # Proprietary libraries
│   └── nxcore/
│       └── libnx.so
└── tests/                               # Unit/integration tests (optional/expandable)
```

## Building & Installing
Out-of-source build is enforced.  The install rule copies libnx.so for convenience in demo deployments; in production one would typically use rpath or system package management.  

From the main project directory (`NxCoreApiTapeFileReader/`):
```bash
# Configure + generate build system (creates/updates the 'build' folder)
rm -rf build && mkdir build
cmake -S . -B build -G Ninja -DCMAKE_INSTALL_PREFIX=/opt/nxcore-demo

# Alternative if you want to force Debug/Release or other settings:
# cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON -G Ninja

# Build the project (parallel build using all cores)
cmake --build build --parallel

# Or more explicit (same effect):
# cmake --build build -j$(nproc)

# Install usually needs sudo when installing to /usr/local (or /usr)
sudo cmake --install build

# After installation, update the dynamic linker cache
sudo ldconfig
ldconfig -p | grep libnx.so
```
#### Custom Install Prefix (no sudo needed for local testing)
To install to a non-standard location:
```bash
cmake -S . -B build -G Ninja -DCMAKE_INSTALL_PREFIX=$PWD/install
cmake --build build --parallel
cmake --install build
```
Then run locally:
```bash
./install/bin/nxcore-tape-reader libnx.so /path/to/your.tape.nx2
```

#### Installed Files (default prefix /usr/local):
- Binary: `/usr/local/bin/nxcore-tape-reader`
- Static Library: `/usr/local/lib/libtapefilereaderproject.a`
- Library: `/usr/local/lib/extern/nxcore/libnx.so`
- CMake config: `/usr/local/lib/cmake/TapeFileReaderProject/*.cmake`

## Running the Application
After installation and `sudo ldconfig`, the loader finds `libnx.so` by name alone (no full path needed):
```bash
nxcore-tape-reader libnx.so /path/to/tape/file.nx2
```
Or during development (from build directory):
```bash
./nxcore-tape-reader ../extern/nxcore/libnx.so /path/to/your.tape.nx2
```

### Usage without arguments (help output)
```bash
nxcore-tape-reader
NxCore Tape Reader (modernized C++17 version)

Usage:
  nxcore-tape-reader <path-to-libnx.so> [path-to-tape-file]

Examples:
  nxcore-tape-reader ./libnx.so /data/tapes/20250102.nxc
  nxcore-tape-reader ./libnx.so      # process live feed
```

### Sample program output from reading a tapefile
```bash
nxcore-tape-reader libnx.so ~/20211013.WE.nx2
Running from: NxCoreApiTapeFileReader (/home/crymoney/Crymoney/CrymoneyCodeLinux/NxCoreApi001TapeFileReader/NxCoreApiTapeFileReader)
Reading from tapefile: /home/crymoney/Crymoney/CrymoneyTest/NanexNxCoreDataSets/20211013.WE.nx2
nxcore-tape-reader starting...
NxCore library loaded successfully.
Processing tape: /home/crymoney/Crymoney/CrymoneyTest/NanexNxCoreDataSets/20211013.WE.nx2
NxCore Initialize Message.
libnx.so version is v3.2.18
NxCore Unknown (default) Status Message: TapeOpen,0,0 (0)
NxCore System symbol spin state: StatusData == 0 Starting symbol spin...
NxCore System symbol spin state: StatusData == 1 Symbol spin complete.
NxCore Time: 10/13/2021 00:01:00
NxCore Time: 10/13/2021 00:02:00
NxCore Time: 10/13/2021 00:03:00
  .
  .  ( and so on ... )
  .
NxCore Time: 10/13/2021 23:58:00
NxCore Time: 10/13/2021 23:59:00
NxCore Time: 10/13/2021 24:00:00
NxCore tape file sentinel read -> hour of the day == 24.
NxCore Complete Message.
Tape completed normally.
nxcore-tape-reader finished.
Execution finished successfully.
```

## Troubleshooting
- If you get "cannot open shared object file: No such file or directory": Ensure `libnx.so` path is correct and re-run `sudo ldconfig` if the library is installed system-wide.
- Permission denied when opening tape file: Check file permissions, ownership, and path.
- Callback not firing: Verify tape file is valid NxCore format (.nx2 or .nx3).
- `libnx.so`: cannot open shared object file even after install → Check that the binary was built with proper INSTALL_RPATH (run chrpath -l /usr/local/bin/nxcore-tape-reader or readelf -d | grep RPATH). If missing, rebuild with rpath settings.

## Planned / Possible Extensions
- Unit tests for callback logic (using CppUnit or GoogleTest)
- Command-line options (tape file list, symbol filters, output format)
- Multi-threaded or async tape processing
- Export to CSV/Parquet/Arrow
- Docker container for reproducible builds

## Important Notices
- This project links against a proprietary library (libnx.so) and requires a valid subscription/license from Nanex to obtain it and to process real tape files.
- Tape file processing requires compatible .nx2 or .nx3 tape files from Nanex historical data.
- No warranty is provided. Use at your own risk.
- For official NxCore API documentation, refer to https://www.nxcoredata.com/ or https://nxcoreapi.com/doc/ (as available).
  
## Licensing
- **Proprietary components** (libnx.so, NxCoreAPI.h, original Sample1 logic):
© Nanex / NxCore Market Data — all rights reserved.
Must be obtained legally from https://www.nxcoredata.com/. Not redistributed here.
- **This project's additions and modifications** (CMake build system, project structure, wrapper code, main.cpp adaptations, documentation):
Copyright © R. Zanecki / 2025 - present
Licensed under the **MIT License**. See the LICENSE.md file for full terms.

## References
- NxCore Market Data by Nanex: https://www.nxcoredata.com/
- API Overview: https://www.nxcoredata.com/nxcore-api-overview
- API Downloads: https://www.nxcoredata.com/nxcore-downloads
- Full API Documentation: https://nxcoreapi.com/doc/
- Sample Data: https://www.nxcoredata.com/sample-nxcore-data