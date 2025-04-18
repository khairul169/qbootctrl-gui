cmake_minimum_required(VERSION 3.10)
project(qbootctl-gtk C)

set(CMAKE_C_STANDARD 99)

# GTK
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Include GTK and third-party includes
include_directories(
    ${GTK3_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/includes
    ${CMAKE_SOURCE_DIR}/lib/qbootctl
)

link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

# Source files
file(GLOB SRC_FILES
    src/*.c
    lib/qbootctl/*.c
)

# Exclude main entry file of qbootctl.c
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/qbootctl/qbootctl.c")

add_executable(qbootctl-gtk ${SRC_FILES})
target_link_libraries(qbootctl-gtk ${GTK3_LIBRARIES})
