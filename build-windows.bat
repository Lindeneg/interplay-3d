@echo off

REM run cmake and use generated ninja/clang toolchain
cmake --build .\build --target interplay -j 18

REM copy dependencies (DLLs and assets) to build folder
copy /Y .\vendor\windows\SDL2-VC-2.30.0\lib\x64\SDL2.dll .\build\bin\
copy /Y .\vendor\windows\SDL2_image-VC-2.8.2\lib\x64\SDL2_image.dll .\build\bin\
copy /Y .\vendor\windows\SDL2_ttf-VC-2.22.0\lib\x64\SDL2_ttf.dll .\build\bin\
