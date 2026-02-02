#!/usr/bin/env bash

# file: run.sh
# Script to run the project app file for testing.
# Only runs if the current directory ends with one of the allowed names

# How to run:
# cd ~/Crymoney/CrymoneyCodeLinux/NxCoreApi001readTapeFile/NxCoreTapeFileReader
# ./run.sh

TAPEFILEPATH="$HOME/Crymoney/CrymoneyTest/NanexNxCoreDataSets/20211013.WE.nx2"

# Optional: verify the file exists (very useful for debugging)
if [[ ! -f "$TAPEFILEPATH" ]]; then
    echo "Error: Tape file not found: $TAPEFILEPATH"
    exit 2
fi

APPFILENAME="nxcore-tape-reader"
ALLOWED1="NxCoreApi001TapeFileReader"      # Parent directory first level
ALLOWED2="NxCoreApiTapeFileReader"         # Subdirectory where this script resides

# ────────────────────────────────────────────────
# Determine which allowed directory we're in
# ────────────────────────────────────────────────

if [[ "$PWD" == *"$ALLOWED1" ]]; then
    CONTEXT=$ALLOWED1
    RELPATH="./$ALLOWED2/build"
elif [[ "$PWD" == *"$ALLOWED2" ]]; then
    CONTEXT=$ALLOWED2
    RELPATH="./build"
else
    echo "Error: This script can only be run from a directory whose full path ends with:"
    echo "  - .../$ALLOWED1"
    echo "  - .../$ALLOWED2"
    echo ""
    echo "Current directory: $PWD"
    echo "Aborting."
    exit 1
fi

echo "Running from: $CONTEXT ($PWD)"

# ────────────────────────────────────────────────
# Run with relative paths (assuming you're in the correct project root)
# ────────────────────────────────────────────────

echo "Reading from tapefile: $TAPEFILEPATH"

$RELPATH/$APPFILENAME libnx.so $TAPEFILEPATH

# Optional: show exit status
if [ $? -eq 0 ]; then
    echo "Execution finished successfully."
else
    echo "Execution failed (exit code $?)"
fi

# End of file run.sh
