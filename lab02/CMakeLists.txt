cmake_minimum_required(VERSION 3.14)
project(lab02)
set(CMAKE_CXX_STANDARD 17)
set(SOURCE_FILES main.cpp Lab02Engine.cpp Lab02Engine.h FreeCam.cpp FreeCam.h Being.h Being.cpp FirstPerson.hpp Car.cpp Car.h)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Windows with MinGW Installations
if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND MINGW )
    # if working on Windows but not in the lab
    # update the include directory location
    include_directories("C:/CSCI441/include")
    # update the lib directory location
    target_link_directories(${PROJECT_NAME} PUBLIC "C:/CSCI441/lib")
    target_link_libraries(${PROJECT_NAME} opengl32 glfw3 glad gdi32)
    # OS X Installations
elseif( APPLE AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    # update the include directory location
    include_directories("/usr/local/include")
    # update the lib directory location
    target_link_directories(${PROJECT_NAME} PUBLIC "/usr/local/lib")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" glfw3 glad)
    # Blanket *nix Installations
elseif( UNIX AND ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    # update the include directory location
    include_directories("/usr/local/include")
    # update the lib directory location
    target_link_directories(${PROJECT_NAME} PUBLIC "/usr/local/lib")
    target_link_libraries(${PROJECT_NAME} GL glfw glad)
endif()