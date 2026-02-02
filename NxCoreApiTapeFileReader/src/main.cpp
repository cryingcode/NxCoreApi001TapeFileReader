// file: main.cpp

////////////////////////////////////////////////////
//// SampleApp1.cpp                             ////
//// Sample App for NxCore in C++               ////
//// Authors: Jeffrey Donovan, Jeremy Truelove  ////
//// Date: 2017-02-08                           ////
////////////////////////////////////////////////////
//// Demonstrates:                              ////
//// 1) Starting NxCore ProcessTape             ////
//// 2) Defining/using NxCore Callback Function ////
//// 3) Handling the NxCore Status Message      ////
////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <cstdlib>
#include <stdexcept>

#include "executableUtils.hpp"
#include "nxcoreCallback.hpp"

#include "srclib/nxcore/NxCoreAPI_Wrapper_C++.h"

NxCoreClass NxCore; // global instance (consider dependency injection in larger apps)

constexpr unsigned int DEFAULT_PROCESS_FLAGS =
    NxCF_EXCLUDE_QUOTES |
    NxCF_EXCLUDE_QUOTES2 |
    NxCF_EXCLUDE_OPRA;

int main(int argc, char *argv[])
{
    using namespace std;

    string exeName;
    try
    {
        exeName = get_executable_filename();
    }
    catch (const exception &e)
    {
        cerr << "Failed to get executable name: " << e.what() << "\n";
        return 1;
    }

    if (argc < 2)
    {
        cout << "Program derived from NxCore API SampleApp1\n";
        cout << exeName << " libnx.so [path to tapefile]\n";
        return EXIT_SUCCESS;
    }

    cout << "NxCore C++ " << exeName << " Start.\n";

    if (!NxCore.LoadNxCore(argv[1]))
    {
        cerr << "Failed to load NxCore Library: " << dlerror() << "\n";
        return EXIT_FAILURE;
    }

    int returnValue = 0;
    if (argc >= 3)
    {
        cout << "Processing the tape: " << argv[2] << "\n";
        returnValue = NxCore.ProcessTape(argv[2], nullptr, DEFAULT_PROCESS_FLAGS, 0,
                                         reinterpret_cast<NxCoreCallback>(OnNxCoreCallback));
    }
    else
    {
        returnValue = NxCore.ProcessTape("", nullptr, DEFAULT_PROCESS_FLAGS, 0,
                                         reinterpret_cast<NxCoreCallback>(OnNxCoreCallback));
    }

    NxCore.ProcessReturnValue(returnValue);
    NxCore.UnloadNxCore();

    cout << "NxCore C++ " << exeName << " Stop.\n";
    return EXIT_SUCCESS;
}