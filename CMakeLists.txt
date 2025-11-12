cmake_minimum_required(VERSION 3.21)
project(DonRoute)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

include_directories(
        ${CMAKE_SOURCE_DIR}/header
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/ui
)

# --- Agrega el ejecutable ---
qt_add_executable(DonRoute
        main.cpp
        src/mainview.cpp
        header/mainview.h
        ui/mainview.ui
)

# --- Agrega el archivo de recursos ---
qt_add_resources(RESOURCES resources/styles/resources.qrc)
target_sources(DonRoute PRIVATE ${RESOURCES})

# --- Enlaza librerías Qt ---
target_link_libraries(DonRoute
        PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)
