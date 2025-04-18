cmake_minimum_required(VERSION 3.10)
project(qbootctl-gui C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Check for linux/bsg.h
include(CheckIncludeFile)
check_include_file("linux/bsg.h" HAVE_LINUX_BSG_H)
if(NOT HAVE_LINUX_BSG_H)
    message(FATAL_ERROR "linux-headers not found: linux/bsg.h is missing")
endif()

# Includes
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

add_executable(qbootctl-gui ${SRC_FILES})
target_link_libraries(qbootctl-gui ${GTK3_LIBRARIES})

# Install app
install(TARGETS qbootctl-gui
    RUNTIME DESTINATION bin
)
install(FILES misc/qbootctl-gui.desktop
    DESTINATION /usr/share/applications
)

# Uninstall
add_custom_target("uninstall" COMMENT "Uninstall installed files")
add_custom_command(
    TARGET "uninstall"
    POST_BUILD
    COMMENT "Uninstall files with install_manifest.txt"
    COMMAND xargs rm -vf < install_manifest.txt || echo Nothing in install_manifest.txt to be uninstalled!
)
