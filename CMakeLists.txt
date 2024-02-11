cmake_minimum_required(VERSION 3.27)
project(interplay C)

set(SDL2_PATH "D:/SDL/SDL2-VC-2.30.0")
set(SDL2_IMAGE_PATH "D:/SDL/SDL2_image-VC-2.8.2")
set(SDL2_TTF_PATH "D:/SDL/SDL2_ttf-VC-2.22.0")
set(MICRO_UI_PATH "D:/dev/interplay/microui/src")

set(CMAKE_C_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/interplay)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

include_directories("${SDL2_PATH}/include"
        "${SDL2_IMAGE_PATH}/include"
        "${SDL2_TTF_PATH}/include"
        "${MICRO_UI_PATH}")

add_executable(interplay src/main.c ${MICRO_UI_PATH}/microui.c
        src/graphics/screen.c
        src/graphics/screen.h
        src/math/vec.c
        src/math/vec.h
        src/globals.h
        src/utils.c
        src/utils.h
        src/log.c
        src/log.h
        src/entities/entity.h
        src/core/mem.c
        src/core/mem.h
        src/core/text.c
        src/core/text.h
        src/core/manager.c
        src/core/manager.h
        src/graphics/renderer.c
        src/graphics/renderer.h
        src/ui.c
        src/ui.h
        src/math/mat4.c
        src/math/mat4.h
        src/core/linked-list.c
        src/core/linked-list.h
        src/core/array.c
        src/core/array.h
        src/core/context.c
        src/core/context.h
        src/graphics/triangle.h
        src/core/texture.c
        src/core/texture.h
        src/entities/mesh.c
        src/entities/mesh.h
        src/graphics/clipping.c
        src/graphics/clipping.h
        src/graphics/light.c
        src/graphics/light.h
        src/graphics/camera.c
        src/graphics/camera.h
        src/graphics/triangle.c
)

target_link_libraries(interplay "${SDL2_PATH}/lib/x64/SDL2main.lib"
        "${SDL2_PATH}/lib/x64/SDL2.lib"
        "${SDL2_IMAGE_PATH}/lib/x64/SDL2_image.lib"
        "${SDL2_TTF_PATH}/lib/x64/SDL2_ttf.lib")


