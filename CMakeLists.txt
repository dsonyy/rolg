cmake_minimum_required(VERSION 3.13)

# Define project name macro
set(PROJECT_NAME "rogue")

project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find PkgConfig
find_package(PkgConfig)

# Find SDL2 library
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# Set include and source directories
set(SRC_DIR src)
set(INCLUDE_DIR include)

# Add source files
add_executable(${PROJECT_NAME} ${SRC_DIR}/main.cpp)

# Include directories
target_include_directories(
    ${PROJECT_NAME} 
    PRIVATE 
    ${INCLUDE_DIR} 
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

# Link with SDL2 library
target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE 
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)