cmake_minimum_required(VERSION 3.16)
project(2DFactorio)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# nur Quell-Dateien
file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Includes nur für dieses Target
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/External/include
)

# Beispiel: statische libs aus External/lib
target_link_libraries(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/External/lib/box2d.lib"
    "${CMAKE_SOURCE_DIR}/External/lib/ChipmunkLib.lib"
    glfw3.lib
)