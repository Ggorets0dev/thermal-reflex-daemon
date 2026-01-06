cmake_minimum_required(VERSION 3.16)

project(threfd LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
SET(CMAKE_C_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB2 REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(JSONC REQUIRED IMPORTED_TARGET json-c)
pkg_check_modules(SYSTEMD REQUIRED IMPORTED_TARGET libsystemd)

file(GLOB_RECURSE SRC_FILES "src/*.c")
file(GLOB_RECURSE INC_FILES "inc/*.h")

add_executable(threfd ${SRC_FILES})

target_include_directories(threfd PRIVATE
        "inc")

target_link_libraries(threfd PRIVATE
        PkgConfig::GLIB2
        PkgConfig::JSONC
        PkgConfig::SYSTEMD
        wiringPi)
