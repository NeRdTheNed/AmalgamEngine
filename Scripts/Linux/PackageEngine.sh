#!/bin/bash

if [[ $# -eq 0 ]] ; then
    echo "Packages the client, server, and sprite editor binaries alongside necessary resources and shared libs."
    echo "Usage: ./PackageEngine <Debug/Release>"
    exit 1
fi

# Grab the full path to the base of the repo (assuming this script is in Scripts/Linux)
BasePath="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../.." >/dev/null 2>&1 && pwd )"

# Check that the build files exist.
Config=$1
BuildPath=$BasePath/Build/Linux/$Config
if ! [ -f "$BuildPath/Server/Server" ] || ! [ -f "$BuildPath/Client/Client" ] \
   ! [ -f "$BuildPath/SpriteEditor/SpriteEditor" ] ; then
    echo "Build files for $Config config were not found. Please build before attempting to package."
    exit 1
fi

# Make the directories that we're going to use.
PackagePath=$BasePath/Output/Linux/$Config
mkdir -p $PackagePath/Client
mkdir -p $PackagePath/Server
mkdir -p $PackagePath/SpriteEditor

# Clear out the package directories to prep for the new files.
rm -r $PackagePath/Client/* >/dev/null 2>&1
rm -r $PackagePath/Server/* >/dev/null 2>&1
rm -r $PackagePath/SpriteEditor/* >/dev/null 2>&1

echo "Starting packaging process."

# Copy the binaries.
cp $BuildPath/Client/Client $PackagePath/Client/
cp $BuildPath/Server/Server $PackagePath/Server/
cp $BuildPath/SpriteEditor/SpriteEditor $PackagePath/SpriteEditor/

# Copy the resource files to the client and sprite editor.
cp -r $BasePath/Resources/Client $PackagePath/Client/
cp -r $BasePath/Resources/SpriteEditor $PackagePath/SpriteEditor/

# Detect and copy dependencies.
cmake -P $BasePath/CMake/copy_runtime_deps.cmake $BuildPath/Client/Client $PackagePath/Client/
cmake -P $BasePath/CMake/copy_runtime_deps.cmake $BuildPath/Server/Server $PackagePath/Server/
cmake -P $BasePath/CMake/copy_runtime_deps.cmake $BuildPath/Server/SpriteEditor $PackagePath/SpriteEditor/

# Set the rpath of our executable and dependencies to $ORIGIN (effectively ./)
find $PackagePath/Client/ -maxdepth 1 -type f -exec patchelf --set-rpath '$ORIGIN' {} \;
find $PackagePath/Server/ -maxdepth 1 -type f -exec patchelf --set-rpath '$ORIGIN' {} \;
find $PackagePath/SpriteEditor/ -maxdepth 1 -type f -exec patchelf --set-rpath '$ORIGIN' {} \;
    
