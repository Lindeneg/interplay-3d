cmake_minimum_required(VERSION 3.27)
project(interplay C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

if (APPLE)
    find_library(SDL2_LIBRARY SDL2 PATHS "${CMAKE_SOURCE_DIR}/vendor/macos")
    find_library(SDL2_IMAGE_LIBRARY SDL2_image PATHS "${CMAKE_SOURCE_DIR}/vendor/macos")
    find_library(SDL2_TTF_LIBRARY SDL2_ttf PATHS "${CMAKE_SOURCE_DIR}/vendor/macos")

    include_directories("${CMAKE_SOURCE_DIR}/vendor/macos/SDL2.framework/Headers")
    include_directories("${CMAKE_SOURCE_DIR}/vendor/macos/SDL2_image.framework/Headers")
    include_directories("${CMAKE_SOURCE_DIR}/vendor/macos/SDL2_ttf.framework/Headers")
endif()

if(WIN32)
    include_directories("${CMAKE_SOURCE_DIR}/vendor/windows/SDL2-VC-2.30.0/include")
    include_directories("${CMAKE_SOURCE_DIR}/vendor/windows/SDL2_image-VC-2.8.2/include")
    include_directories("${CMAKE_SOURCE_DIR}/vendor/windows/SDL2_ttf-VC-2.22.0/include")
endif()

include_directories("${CMAKE_SOURCE_DIR}/microui/src")

add_executable(interplay src/main.c ./microui/src/microui.c
        src/globals.h
        src/compat.c
        src/compat.h
        src/utils.c
        src/utils.h
        src/log.c
        src/log.h
        src/ui.c
        src/ui.h
        src/math/vec.c
        src/math/vec.h
        src/math/mat4.c
        src/math/mat4.h
        src/entities/entity.h
        src/entities/mesh.c
        src/entities/mesh.h
        src/core/linked-list.c
        src/core/linked-list.h
        src/core/array.c
        src/core/array.h
        src/core/context.c
        src/core/context.h
        src/core/mem.c
        src/core/mem.h
        src/core/text.c
        src/core/text.h
        src/core/manager.c
        src/core/manager.h
        src/core/texture.c
        src/core/texture.h
        src/graphics/screen.c
        src/graphics/screen.h
        src/graphics/renderer.c
        src/graphics/renderer.h
        src/graphics/triangle.c
        src/graphics/triangle.h
        src/graphics/clipping.c
        src/graphics/clipping.h
        src/graphics/light.c
        src/graphics/light.h
        src/graphics/camera.c
        src/graphics/camera.h
)

if (APPLE)
    target_link_libraries(interplay ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY})
endif()

if(WIN32)
    target_link_libraries(interplay "${CMAKE_SOURCE_DIR}/vendor/windows/SDL2-VC-2.30.0/lib/x64/SDL2main.lib"
    "${CMAKE_SOURCE_DIR}/vendor/windows/SDL2-VC-2.30.0/lib/x64/SDL2.lib"
    "${CMAKE_SOURCE_DIR}/vendor/windows/SDL2_image-VC-2.8.2/lib/x64/SDL2_image.lib"
    "${CMAKE_SOURCE_DIR}/vendor/windows/SDL2_ttf-VC-2.22.0/lib/x64/SDL2_ttf.lib")
endif()
