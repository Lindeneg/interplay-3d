@echo off

REM run cmake and use generated ninja/clang toolchain
echo building project
cmake --build .\build --target interplay -j 18

REM copy dependencies (DLLs and assets) to build folder
echo copying DLL dependencies
copy /Y .\lib\*.dll .\build\interplay\

REM set custom environment variables
env

