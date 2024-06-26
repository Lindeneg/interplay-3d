@echo off

REM run cmake and use generated ninja/clang toolchain
cmake --build .\build --target interplay -j 18

REM copy dependencies (DLLs and assets) to build folder
copy /Y .\libs\SDL2\windows\SDL2.dll .\build\bin\
copy /Y .\libs\SDL2\windows\SDL2_image.dll .\build\bin\
copy /Y .\libs\SDL2\windows\SDL2_ttf.dll .\build\bin\

REM setup 'r' key to exec run script
REM I use this personally as a key-binding in nvim
DOSKEY r=call "./misc/run.bat"
