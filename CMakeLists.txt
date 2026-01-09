cmake_minimum_required(VERSION 4.1)
project(IconfontViewer VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

message(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
message(STATUS "MSVC_VERSION = ${MSVC_VERSION}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_PREFIX_PATH "C:/Qt/6.9.3/msvc2022_64/lib/cmake")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_PREFIX_PATH "C:/Qt/6.9.3/mingw_64/lib/cmake")
endif ()

file(GLOB ALL_SOURCE "*.cpp" "*.c")
file(GLOB ALL_HEADER "*.hpp" "*.h")
file(GLOB ALL_RESOURCES "*.qrc")

set(QT_COMPONENTS
        Core
        Gui
        Widgets
)

set(QT_LIBRARIES)

foreach (item IN LISTS QT_COMPONENTS)
    list(APPEND QT_LIBRARIES Qt::${item})
endforeach ()

find_package(Qt6 COMPONENTS
        ${QT_COMPONENTS}
        REQUIRED)

find_package(freetype REQUIRED CONFIG)

add_executable(${PROJECT_NAME}
        ${ALL_SOURCE}
        ${ALL_HEADER}
        ${ALL_RESOURCES}
)

target_link_libraries(${PROJECT_NAME}
        ${QT_LIBRARIES}
        Freetype::Freetype
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(${PROJECT_NAME} PROPERTIES
            WIN32_EXECUTABLE TRUE
    )
endif ()

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${QT_INSTALL_PATH}/bin/windeployqt.exe
            "$<TARGET_FILE:${PROJECT_NAME}>"
            --no-translations
    )
endif ()
