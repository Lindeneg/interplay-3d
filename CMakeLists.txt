cmake_minimum_required(VERSION 3.27)
project(interplay C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_compile_options(-ggdb -pedantic-errors -D DEBUG -Wall)

set(MICRO_UI_PATH "${CMAKE_SOURCE_DIR}/vendor/microui/src")

include_directories("${CMAKE_SOURCE_DIR}/includes/")
include_directories(${MICRO_UI_PATH})

file(GLOB_RECURSE PROJECT_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} src/*.c)

add_executable(interplay ${PROJECT_SOURCES} "${MICRO_UI_PATH}/microui.c")

if (APPLE OR UNIX)
    set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    target_link_libraries(interplay ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})
elseif(WIN32)
    target_link_libraries(interplay "${CMAKE_SOURCE_DIR}/libs/SDL2/windows/SDL2main.lib"
        "${CMAKE_SOURCE_DIR}/libs/SDL2/windows/SDL2.lib"
        "${CMAKE_SOURCE_DIR}/libs/SDL2/windows/SDL2_image.lib"
        "${CMAKE_SOURCE_DIR}/libs/SDL2/windows/SDL2_ttf.lib")
endif()
