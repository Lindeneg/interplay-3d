@echo off

REM generate build pipeline in build folder
cmake -S . -B .\build -G Ninja -DCMAKE_BUILD_TYPE=Debug "-DCMAKE_MAKE_PROGRAM=ninja.exe" -DCMAKE_BUILD_TYPE=Debug

REM copy new compile_commands.json for clangd LSP
copy /Y .\build\compile_commands.json .

REM setup 'b' key to exec build script
REM I use this personally as a key-binding in nvim
DOSKEY b=call "./misc/build.bat"
