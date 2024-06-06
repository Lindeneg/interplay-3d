@echo off

:: Remove previous build files
@RD /S /Q ".\obj"
@RD /S /Q ".\build"

SETLOCAL

:: Ensure SDL libs are downloaded and extracted. Set paths accordingly.

:: https://github.com/libsdl-org/SDL/releases/download/release-2.30.0/SDL2-devel-2.30.0-VC.zip
SET SDL2_PATH=D:\SDL\SDL2-VC-2.30.0
:: https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/SDL2_image-devel-2.8.2-VC.zip
SET SDL2_IMAGE_PATH=D:\SDL\SDL2_image-VC-2.8.2
:: https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-devel-2.22.0-VC.zip
SET SDL2_TTF_PATH=D:\SDL\SDL2_ttf-VC-2.22.0

:: Already included as git submodule
SET MICRO_UI_PATH=.\microui\src

:: Set object directory
SET OBJ_DIR=obj
MKDIR %OBJ_DIR%

:: Set source files
SET SRC_FILES=src\main.c ^
    %MICRO_UI_PATH%\microui.c ^
    src\utils.c src\log.c src\ui.c src\compat.c ^
    src\math\vec.c src\math\mat4.c ^
    src\entities\mesh.c ^
    src\core\linked-list.c src\core\array.c src\core\context.c src\core\mem.c src\core\text.c src\core\manager.c src\core\texture.c ^
    src\graphics\screen.c src\graphics\renderer.c src\graphics\triangle.c src\graphics\clipping.c src\graphics\light.c src\graphics\camera.c

:: Set output directory
SET OUTPUT_DIR=build
MKDIR %OUTPUT_DIR%

:: Set compiler options
SET SDL2_LIB="%SDL2_PATH%\lib\x64"
SET SDL2_IMAGE_LIB="%SDL2_IMAGE_PATH%\lib\x64"
SET SDL2_TTF_LIB="%SDL2_TTF_PATH%\lib\x64"

set COMPILER_FLAGS=/EHsc /Fo%OBJ_DIR%\ /Fe%OUTPUT_DIR%\interplay.exe

set COMPILER_INCLUDE=/I "%SDL2_PATH%/include" /I "%SDL2_IMAGE_PATH%/include" /I "%SDL2_TTF_PATH%/include" /I "%MICRO_UI_PATH%"

set COMPILER_LINK=/link "%SDL2_LIB%\SDL2main.lib" "%SDL2_LIB%\SDL2.lib" "%SDL2_IMAGE_LIB%\SDL2_image.lib" "%SDL2_TTF_LIB%\SDL2_ttf.lib"

:: Compile the program
cl %COMPILER_INCLUDE% %SRC_FILES% %COMPILER_FLAGS% %COMPILER_LINK%

:: Copy SDL2 DLL files
COPY /Y "%SDL2_LIB%\SDL2.dll" ".\%OUTPUT_DIR%\"
COPY /Y "%SDL2_IMAGE_LIB%\SDL2_image.dll" ".\%OUTPUT_DIR%\"
COPY /Y "%SDL2_TTF_LIB%\SDL2_ttf.dll" ".\%OUTPUT_DIR%\"

ENDLOCAL

