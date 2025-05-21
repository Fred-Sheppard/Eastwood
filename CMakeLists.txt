cmake_minimum_required(VERSION 3.16)
project(Eastwood VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt auto features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets UiTools)

# Find Crypto++
find_path(CRYPTOPP_INCLUDE_DIR cryptopp/cryptlib.h
    PATHS /opt/homebrew/include
    NO_DEFAULT_PATH
)
find_library(CRYPTOPP_LIBRARY
    NAMES cryptopp
    PATHS /opt/homebrew/lib
    NO_DEFAULT_PATH
)

# Define sources
set(FRONTEND_SOURCES
    frontend/windows/login/login.cpp
    frontend/windows/login/login.h
    frontend/windows/login/login.ui
    frontend/windows/register/register.cpp
    frontend/windows/register/register.h
    frontend/windows/register/register.ui
    frontend/utils/messagebox.h
)

add_executable(Eastwood
    main.cpp
    ${FRONTEND_SOURCES}
)

# Include directories and link libraries
target_include_directories(Eastwood PRIVATE ${CRYPTOPP_INCLUDE_DIR})
target_link_libraries(Eastwood PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::UiTools
    ${CRYPTOPP_LIBRARY}
)
