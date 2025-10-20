cmake_minimum_required(VERSION 3.20)

project(Chip8Emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(functions)
get_version_from_git(GIT_VERSION)

# Dependencies -----------------------------------------------------------------

include(FetchContent)

message(STATUS "Installing dependency cxxopts...")
FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG        v3.3.1
)
FetchContent_MakeAvailable(cxxopts)

message(STATUS "Installing dependency SDL2...")
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-2.30.10
)
FetchContent_MakeAvailable(SDL2)

# Sources and Targets ----------------------------------------------------------

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp")
add_library(Chip8Emulator_Core ${SOURCES} ${HEADERS})

target_link_libraries(Chip8Emulator_Core
                      PUBLIC cxxopts::cxxopts
                             SDL2::SDL2
                             SDL2::SDL2main)

target_compile_definitions(Chip8Emulator_Core
                           PUBLIC PROGRAM_NAME="${PROJECT_NAME}"
                                  PROGRAM_VERSION="${GIT_VERSION}")

add_executable(Chip8Emulator src/main.cpp)
target_link_libraries(Chip8Emulator PRIVATE Chip8Emulator_Core)

if(NOT CPACK_GENERATOR MATCHES "DEB|RPM")
    set_target_properties(Chip8Emulator PROPERTIES
        INSTALL_RPATH "$ORIGIN/../lib"
        BUILD_WITH_INSTALL_RPATH FALSE)
endif()

# Installation -----------------------------------------------------------------

install(TARGETS Chip8Emulator
        RUNTIME DESTINATION bin
        COMPONENT runtime)

# The DEB and RPM packages handle the SDL2 dependency on their own.
if(NOT CPACK_GENERATOR MATCHES "DEB|RPM")
    install(TARGETS SDL2
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin
            COMPONENT runtime)
endif()

# Packaging --------------------------------------------------------------------

string(REGEX REPLACE "^v" "" PACKAGE_VERSION "${GIT_VERSION}")

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PACKAGE_VERSION}")
set(CPACK_PACKAGE_VENDOR "mparati")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CHIP-8 Emulator")
set(CPACK_PACKAGE_DESCRIPTION "A CHIP-8 emulator implementation")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_GENERATOR "TGZ;ZIP;STGZ;TBZ2;TXZ;TZ;DEB;RPM")

# DEB-specific settings
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "mparati")
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl2-2.0-0")

# RPM-specific settings
set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0")
set(CPACK_RPM_PACKAGE_GROUP "Amusements/Games")
set(CPACK_RPM_PACKAGE_REQUIRES "SDL2")

include(CPack)
