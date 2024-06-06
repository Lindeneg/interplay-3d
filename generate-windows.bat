@echo off

REM generate build pipeline in build folder
cmake -S . -B .\build -G Ninja -DCMAKE_BUILD_TYPE=Debug "-DCMAKE_MAKE_PROGRAM=ninja.exe" -DCMAKE_BUILD_TYPE=Debug

REM copy new compile_commands.json for clangd LSP
copy /Y .\build\compile_commands.json .
